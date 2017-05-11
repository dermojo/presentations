/**
 * Digitally sign (and verify) scripts for fun and profit.
 */

#include "signatures.hpp"

#include <cstring>
#include <fstream>
#include <sys/stat.h>

#include <openssl/pem.h>
#include <openssl/rsa.h>

/// the private key file: always read from disk
static const char* const PRIVATE_KEY_FILE = "privkey.pem";

/// public key file *content*: compiled into the executable to avoid additional files
static const char PUBLIC_KEY[] = "\
-----BEGIN PUBLIC KEY-----\
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyRyx68XFJ9XaJoRd541y\
A5RyyemegMCn1/GOg0nTJzyXmSqAzQOJll9nSXwRvZxoSflW96S3vH3Tx0tTnPfJ\
HDDV5cpWqGLdO0zgGq4Uy7AZz0qd71aqftyFCWf1koM17VzcqJ0S+70tnSEaAs2I\
UYIftOcPgIy/kelekXOpEEKZdyl7rLpyv+CGCd+YOn5CD8SnC1zYB3aVkwP1oGd2\
Ucm73MA13oMPyK4HWfi9WPnFfrPg/i90rFDOUMsIQuK6lQgzvzo+pxfyxZ4uyp0E\
T1v6zWKWdXEpbcoMpa0pXmQ6ojNfY3E2FvPWo76vYI3PFr7/T5lIKXHw6GviJ6ft\
qwIDAQAB\
-----END PUBLIC KEY-----";

static constexpr size_t HEADER_MAGIC_SIZE = 8;

/// header magic
static const char HEADER_MAGIC[HEADER_MAGIC_SIZE + 1] = "**MYPY**";

static constexpr size_t SIGNATURE_SIZE = 256;

/**
 * Stand alone script header format: consists of a simple magic, identifying the file type,
 * and a signature.
 * Feel free to add more properties, e.g. validity time stamps, host IDs etc.
 */
struct StandaloneHeader
{
    char magic[HEADER_MAGIC_SIZE];
    unsigned char signature[SIGNATURE_SIZE];
};

bool isStandalone(const char* file)
{
    char buffer[HEADER_MAGIC_SIZE]{};

    std::ifstream ifs(file);
    if (ifs.is_open())
    {
        // skip the first line, it's the shebang
        std::string shebang;
        std::getline(ifs, shebang);
        ifs.read(buffer, sizeof(buffer));
    }
    return memcmp(buffer, HEADER_MAGIC, HEADER_MAGIC_SIZE) == 0;
}

static bytestring readFile(std::ifstream& ifs)
{
    bytestring content;
    char buffer[512];
    while (ifs.good())
    {
        auto n = ifs.readsome(buffer, sizeof(buffer));
        if (!n)
            break;
        content.append((unsigned char*)buffer, n);
    }

    return content;
}

bytestring readFile(const char* file)
{
    std::ifstream ifs(file);
    if (!ifs.is_open())
        throw std::runtime_error(strerror(errno));

    return readFile(ifs);
}

static RSA* readPrivateKey()
{
    FILE* pk = fopen(PRIVATE_KEY_FILE, "r");
    if (!pk)
        throw std::runtime_error("Failed to open private key file!");

    RSA* privkey = RSA_new();
    if (!PEM_read_RSAPrivateKey(pk, &privkey, nullptr, nullptr))
    {
        RSA_free(privkey);
        throw std::runtime_error("Invalid private key file!");
    }

    return privkey;
}

bytestring sign(const bytestring& data)
{
    RSA* privkey = readPrivateKey();

    unsigned char sigbuf[SIGNATURE_SIZE];
    unsigned int siglen = 0;
    int rc = RSA_sign(NID_sha256, data.data(), data.size(), sigbuf, &siglen, privkey);
    RSA_free(privkey);

    if (rc != 1)
        throw std::runtime_error("RSA_sign() failed :-(");

    if (siglen != SIGNATURE_SIZE)
        throw std::runtime_error("WTF?");

    return bytestring(sigbuf, siglen);
}

void createDetachedSignature(const char* file)
{
    std::string signaturePath = file;
    signaturePath += ".signature";

    const auto content = readFile(file);
    const auto signature = sign(content);

    std::ofstream sigfile(signaturePath);
    if (!sigfile.is_open())
        throw std::runtime_error(strerror(errno));

    sigfile.write((const char*)signature.data(), signature.size());
    sigfile.flush();
}

void makeStandalone(const char* zipFile, const char* outFile)
{
    // create a header
    StandaloneHeader header;
    memcpy(header.magic, HEADER_MAGIC, HEADER_MAGIC_SIZE);

    auto zipContent = readFile(zipFile);
    auto signature = sign(zipContent);
    if (signature.size() != sizeof(header.signature))
        throw std::runtime_error("signature size mismatch");
    memcpy(header.signature, signature.data(), signature.size());

    std::ofstream ofs(outFile);
    if (!ofs.is_open())
        throw std::runtime_error(strerror(errno));

    ofs << "#!/usr/bin/env mypy\n";
    ofs.write((const char*)&header, sizeof(header));
    ofs.write((const char*)zipContent.data(), zipContent.size());
    ofs.close();

    chmod(outFile, 0777);
}

static SignatureStatus verifySignature(const bytestring& sig, const bytestring& data)
{
    RSA* privkey = readPrivateKey();

    int rc = RSA_verify(NID_sha256, data.data(), data.size(), sig.data(), sig.size(), privkey);
    RSA_free(privkey);

    if (rc == 1)
        return SignatureStatus::VALID;
    return SignatureStatus::INVALID;
}

SignatureStatus checkStandaloneSignature(const char* file)
{
    try
    {
        auto content = readFile(file);
        // remove the first line
        size_t pos = content.find('\n');
        if (pos != std::string::npos)
            content.erase(0, pos + 1);
        if (content.size() >= sizeof(StandaloneHeader))
        {
            // separate the header
            StandaloneHeader header;
            memcpy(&header, content.data(), sizeof(header));
            content.erase(0, sizeof(header));

            // verify the rest
            bytestring signature(header.signature, sizeof(header.signature));
            return verifySignature(signature, content);
        }
    }
    catch (std::exception&)
    {
    }
    return SignatureStatus::INVALID;
}

SignatureStatus checkDetachedSignature(const char* file)
{
    std::string signaturePath = file;
    signaturePath += ".signature";

    // Does the file exist?
    std::ifstream ifs(signaturePath);
    if (!ifs.is_open())
    {
        if (errno == ENOENT)
            return SignatureStatus::UNSIGNED;

        // could not be verified... :-(
        return SignatureStatus::INVALID;
    }

    auto signature = readFile(ifs);
    auto content = readFile(file);
    return verifySignature(signature, content);
}
