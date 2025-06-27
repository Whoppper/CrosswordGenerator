#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QVector>
#include <QSqlDatabase>

class DatabaseManager
{
public:
    static DatabaseManager &getInstance();
    bool initializeDatabase();
    bool isEmpty();
    bool fillDB();
    QVector<QString> searchWordByPattern(const QString& userInputPattern);
    void fillWordsList(QVector<QString> &words);

private:
    DatabaseManager();
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H