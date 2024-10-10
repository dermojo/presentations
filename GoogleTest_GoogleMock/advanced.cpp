#include <gtest/gtest.h>

#include <future>

// Sample program:
struct Server
{
    void run(const std::string& addr, uint16_t port);
    void stop();
    uint16_t getListenPort() const;
};

struct Client
{
    std::error_code connect(const std::string& addr, uint16_t port);
    std::error_code list_entries(std::string& response);
    std::error_code get_info(std::string& response);
};

struct Json
{
    bool hasMember(const char*) const;
    Json getMember(const char*) const;
    bool isString() const;
    bool isObject() const;
};

class ServerTest : public testing::Test
{
protected:
    void SetUp() override
    {
        std::promise<uint16_t> promise;
        auto future = promise.get_future();
        serverThread = std::thread(
          [&, this]
          {
              try
              {
                  server.run("127.0.0.1", 0);
                  promise.set_value(server.getListenPort());
              }
              catch (...)
              {
                  promise.set_exception(std::current_exception());
              }
          });
        uint16_t port;
        ASSERT_NO_THROW(port = future.get());
        std::error_code err = client.connect("127.0.0.1", port);
        ASSERT_FALSE(err);
    }
    void TearDown() override
    {
        if (serverThread.joinable())
        {
            server.stop();
            serverThread.join();
        }
    }

    Server server;
    std::thread serverThread;
    Client client;
};

Json parseJson(const std::string&); // throws

static void checkIsValidResponse(const std::string& resp)
{
    auto json = parseJson(resp);
    ASSERT_TRUE(json.isObject());
    ASSERT_TRUE(json.hasMember("message_id"));
    ASSERT_TRUE(json.getMember("message_id").isString());
    // ...
}

TEST_F(ServerTest, list_entries)
{
    std::string resp;
    auto err = client.list_entries(resp);
    ASSERT_FALSE(err);
    checkIsValidResponse(resp);
    // ... specific tests
}

TEST_F(ServerTest, get_info)
{
    std::string resp;
    auto err = client.get_info(resp);
    ASSERT_FALSE(err);
    checkIsValidResponse(resp);
    // ... specific tests
}
