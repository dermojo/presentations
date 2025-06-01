#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::_;
using testing::StrictMock;

bool writeFile(const char* path, std::string_view content)
{
    FILE* f = fopen(path, "rb");
    // ...
    if (fclose(f) != 0)
    {
        // TODO: error handling ... when will this happen?!
    }
    return true;
}

#include <cstdio>

extern "C" int __wrap_fclose(FILE*);
extern "C" int __real_fclose(FILE*);

#if 0
// "raw C" approach
int fclose_error = 0;

int __wrap_fclose(FILE* f)
{
    // really close the file
    EXPECT_EQ(__real_fclose(f), 0) << strerror(errno);
    // return a (potentially) "fake" error
    return fclose_error;
}
#else
// "gmock approach"
class MockFclose
{
public:
    MOCK_METHOD(int, close, (FILE*));
};
MockFclose* fclose_mock = nullptr; // set from your test

int __wrap_fclose(FILE* f)
{
    int rc = __real_fclose(f);
    EXPECT_EQ(rc, 0) << strerror(errno);

    // delegate to the mock (if any)
    if (fclose_mock)
        return fclose_mock->close(f);
    else
        return rc;
}
#endif

struct Stringify
{
    virtual std::string to_string(int value);
    virtual std::string to_string(long value);
    virtual std::string to_string(std::string_view value);
};

struct MockStringify1 : public Stringify
{
    MOCK_METHOD(std::string, to_string, (int value), (override));
    MOCK_METHOD(std::string, to_string, (long value), (override));
    MOCK_METHOD(std::string, to_string, (std::string_view value), (override));
};

struct MockStringify2 : public Stringify
{
    MOCK_METHOD(std::string, to_string_int, (int value));
    std::string to_string(int value) override { return to_string_int(value); }
    MOCK_METHOD(std::string, to_string_long, (long value));
    std::string to_string(long value) override { return to_string_long(value); }
    MOCK_METHOD(std::string, to_string_sv, (std::string_view value));
    std::string to_string(std::string_view value) override { return to_string_sv(value); }
};

struct Json { /* ... */ };

std::optional<int> getMemberAsInt(const Json&, const char*);

struct IClientConnection
{
    virtual ~IClientConnection() = default;
    virtual void sendResponse(int code, const Json& data) = 0;
};
struct MockClientConnection : IClientConnection
{
    MOCK_METHOD(void, sendResponse, (int code, const Json& data), (override));
    MOCK_METHOD(void, sendString, (int code, const std::string& data));
};

TEST(Server, response_version)
{
    StrictMock<MockClientConnection> conn;
    // ...
    EXPECT_CALL(conn, sendResponse(200, _)) .WillOnce([](int, const Json& data) {
         EXPECT_EQ(getMemberAsInt(data, "version"), 1);
    });

    auto getVersion = [](const Json& j) { return getMemberAsInt(j, "version"); };
    EXPECT_CALL(conn, sendResponse(200, testing::ResultOf(getVersion, testing::Eq(1))));

    EXPECT_CALL(conn, sendString(200, testing::Property(&std::string::size, testing::Gt(42))));
}
