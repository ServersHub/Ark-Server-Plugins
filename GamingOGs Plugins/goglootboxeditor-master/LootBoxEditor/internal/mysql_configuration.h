#ifndef MYSQL_CONFIGURATION_H
#define MYSQL_CONFIGURATION_H

#include "json_utils.h"

namespace LootBox
{

    class MySQLConfiguration
    {
    public:
        MySQLConfiguration() = default;
        ~MySQLConfiguration() = default;

        void InitializeEmpty();
        void Load(const JSON& in);

        void SetHost(const QString& val);
        void SetUser(const QString& val);
        void SetPassword(const QString& val);
        void SetDatabase(const QString& val);

        QString GetHost() const;
        QString GetUser() const;
        QString GetPassword() const;
        QString GetDatabase() const;

        JSON Dump() const;

    private:
        QString host_;
        QString user_;
        QString password_;
        QString database_;
    };

}

#endif // MYSQL_CONFIGURATION_H
