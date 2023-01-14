#include "mysql_configuration.h"

namespace LootBox
{

    void MySQLConfiguration::InitializeEmpty()
    {
        host_ = "";
        user_ = "";
        password_ = "";
        database_ = "";
    }

    void MySQLConfiguration::Load(const JSON& in)
    {
        host_ = Utils::ExtractString(in, "MysqlHost");
        user_ = Utils::ExtractString(in, "MysqlUser");
        password_ = Utils::ExtractString(in, "MysqlPass");
        database_ = Utils::ExtractString(in, "MysqlDB");
    }

    void MySQLConfiguration::SetHost(const QString& val)
    {
        host_ = val;
    }

    void MySQLConfiguration::SetUser(const QString& val)
    {
        user_ = val;
    }

    void MySQLConfiguration::SetPassword(const QString& val)
    {
        password_ = val;
    }

    void MySQLConfiguration::SetDatabase(const QString& val)
    {
        database_ = val;
    }

    QString MySQLConfiguration::GetHost() const
    {
        return host_;
    }

    QString MySQLConfiguration::GetUser() const
    {
        return user_;
    }

    QString MySQLConfiguration::GetPassword() const
    {
        return password_;
    }

    QString MySQLConfiguration::GetDatabase() const
    {
        return database_;
    }

    JSON MySQLConfiguration::Dump() const
    {
        return JSON{
            {"MysqlHost", host_.toStdString()},
            {"MysqlUser", user_.toStdString()},
            {"MysqlPass", password_.toStdString()},
            {"MysqlDB", database_.toStdString()}
        };
    }

}
