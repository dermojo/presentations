#pragma once

#include <vector>

class Person { /* ... */ };
inline std::string to_json(const std::vector<Person>&) { return "<serialized persons>"; }

struct IDbConnection
{
    virtual ~IDbConnection() = default;
    virtual std::vector<Person> listPersons() const = 0;
};

struct DbConnection final: IDbConnection
{
    std::vector<Person> listPersons() const final;
};

struct IClientConnection
{
    virtual ~IClientConnection() = default;
    virtual void sendResponse(int code, const std::string& data) = 0;
};

struct ClientConnection final: IClientConnection
{
    void sendResponse(int code, const std::string& data) final;
};

struct Server
{
    explicit Server(std::shared_ptr<IDbConnection> db)
     : m_dbConn(std::move(db)) {}

    void listPersons(IClientConnection& conn)
    {
        try
        {
            std::vector<Person> persons = m_dbConn->listPersons();
            conn.sendResponse(200, to_json(persons));
        }
        catch (const std::exception& exc)
        {
            conn.sendResponse(500, exc.what());
        }
    }

    std::shared_ptr<IDbConnection> m_dbConn;
};
