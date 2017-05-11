/**
 * Digitally sign (and verify) scripts for fun and profit.
 *
 * Key files have been generated using:
 *
 *    openssl genrsa -out privkey.pem
 *    openssl rsa -in privkey.pem -pubout -out pubkey.pem
 */

#pragma once

#include <string>

/**
 * Creates a detached signature (with ".signature" extension).
 * @param[in] file      the file to sign
 */
void createDetachedSignature(const char* file);
/**
 * Creates a standalone script from a ZIP.
 * @param[in] zipFile       the ZIP
 * @param[in] outFile       output file name
 */
void makeStandalone(const char* zipFile, const char* outFile);

/// Signature check results
enum class SignatureStatus
{
    /// valid signature
    VALID,
    /// invalid signature
    INVALID,
    /// unsigned (no signature present)
    UNSIGNED
};

/**
 * @param[in] file      script path
 * @return whether this is a standalone script
 */
bool isStandalone(const char* file);
/**
 * Checks the signature of a standalone script.
 * @param[in] file      script path
 * @return valid/invalid ("unsigned" is not possible here)
 */
SignatureStatus checkStandaloneSignature(const char* file);
/**
 * Checks the detached signature of a script.
 * @param[in] file      script path
 * @return signature status
 */
SignatureStatus checkDetachedSignature(const char* file);

using bytestring = std::basic_string<unsigned char>;

/**
 * Signs some data.
 * @param[in] data      the data to sign
 * @return the signature
 */
bytestring sign(const bytestring& data);
/**
 * @param[in] file  file path
 * @return the file's content
 */
bytestring readFile(const char* file);
