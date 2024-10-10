#include "sample_app2.hpp"

#include <gmock/gmock.h>

struct MockDbConnection : IDbConnection
{
    MOCK_METHOD(std::vector<Person>, listPersons, (), (const override));
};

struct MockClientConnection : IClientConnection
{
    MOCK_METHOD(void, sendResponse, (int code, const std::string& data), (override));
};


TEST(ServerTest, first_mock)
{
    auto dbConn = std::make_shared<MockDbConnection>();
    MockClientConnection conn;

    Server server(dbConn);
    server.listPersons(conn);
}

using testing::StrictMock;
using testing::NiceMock;

TEST(ServerTest, mock_types)
{
    auto dbConn = std::make_shared<StrictMock<MockDbConnection>>();
    NiceMock<MockClientConnection> conn;

    Server server(dbConn);
    server.listPersons(conn);
}

using testing::Return;
using testing::_;

TEST(ServerTest, WHEN_can_list_persons_in_db_THEN_returns_200)
{
    std::vector<Person> samplePersons;
    samplePersons.emplace_back();

    auto dbConn = std::make_shared<StrictMock<MockDbConnection>>();
    EXPECT_CALL(*dbConn, listPersons()).WillOnce(Return(samplePersons));

    StrictMock<MockClientConnection> conn;
    EXPECT_CALL(conn, sendResponse(200, _));

    Server server(dbConn);
    server.listPersons(conn);
}

TEST(ServerTest, WHEN_can_list_persons_in_db_THEN_returns_serialized_persons)
{
    std::vector<Person> samplePersons;
    samplePersons.emplace_back();

    auto dbConn = std::make_shared<StrictMock<MockDbConnection>>();
    EXPECT_CALL(*dbConn, listPersons()).WillOnce(Return(samplePersons));

    StrictMock<MockClientConnection> conn;
    EXPECT_CALL(conn, sendResponse(_, "<serialized persons>"));

    Server server(dbConn);
    server.listPersons(conn);
}

TEST(ServerTest, test_failed_expectation)
{
    std::vector<Person> samplePersons;
    samplePersons.emplace_back();

    auto dbConn = std::make_shared<StrictMock<MockDbConnection>>();
    EXPECT_CALL(*dbConn, listPersons()).WillOnce(Return(samplePersons));

    StrictMock<MockClientConnection> conn;
    EXPECT_CALL(conn, sendResponse(123, _));

    Server server(dbConn);
    server.listPersons(conn);
}