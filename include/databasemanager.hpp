#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class DatabaseManager
{
public:
    static DatabaseManager &getInstance();
    bool initializeDatabase(const QString &path);
    bool isEmpty();
    bool fillDB();
    QList<QString> searchWordByPattern(const QString& userInputPattern);

private:
    DatabaseManager();
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H