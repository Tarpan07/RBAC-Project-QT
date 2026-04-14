#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

class DatabaseManager
{
public:
    static QSqlDatabase database();
    static QString databasePath();

private:
    static void initialize();
    static void createTables();
    static void seedBooks();
    static bool initialized;
};

#endif
