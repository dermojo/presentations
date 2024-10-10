#pragma once

#include <vector>

std::string to_json(auto);

class Person { /* ... */ };

class DbConnection
{
public:
    class Params {};
    std::vector<Person> listPersons();
};

class ClientConnection
{
public:
    void sendResponse(int code, const std::string& data);
};

class Server
{
public:
    explicit Server(const DbConnection::Params&);
    // ...
    void listPersons(ClientConnection& conn)
    {
        try
        {
            std::vector<Person> persons = m_dbConn.listPersons();
            conn.sendResponse(200, to_json(persons));
        }
        catch (const std::exception& exc)
        {
            conn.sendResponse(500, exc.what());
        }
    }
    // ...
private:
    DbConnection m_dbConn;
};