#include "databasemanager.hpp"
#include "logger.hpp"

#include <QInputDialog>
#include <QString>
#include <QMessageBox>

DatabaseManager::DatabaseManager()
{
    
}

DatabaseManager &DatabaseManager::getInstance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initializeDatabase(const QString &path)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);

    if (!db.open())
    {
        Logger::getInstance().log(Logger::LogLevel::Error, "Unable to open database!", "");
        return false;
    }

    QSqlQuery query;

    QString createWordsTable =
    "CREATE TABLE IF NOT EXISTS words ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "word TEXT UNIQUE NOT NULL, "
    "definition BLOB, "
    "hint BLOB);";

    if (!query.exec(createWordsTable))
    {
        Logger::getInstance().log(Logger::LogLevel::Error, "Failed to create words table", "");
        return false;
    }
    if (isEmpty())
    {
        if (!fillDB())
            return false;
    }

    return true;
}


bool DatabaseManager::isEmpty()
{
    QSqlQuery query(db);
    QString countQuery = "SELECT COUNT(*) FROM words;";

    if (!query.exec(countQuery))
    {
        Logger::getInstance().log(Logger::LogLevel::Error, "Failed to execute count query for words table", query.lastError().text());
        return true;
    }
    if (query.next())
    {
        int rowCount = query.value(0).toInt();
        return rowCount == 0;
    }
    return true;
}

bool DatabaseManager::fillDB()
{
    QFile file(":/data/dico.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Logger::getInstance().log(Logger::LogLevel::Error, "Failed to open words file", file.errorString());
        db.close();
        return false;
    }

    QTextStream in(&file);
    int wordsAdded = 0;

    if (!db.transaction())
    {
        Logger::getInstance().log(Logger::LogLevel::Error, "Failed to start database transaction", db.lastError().text());
        return false;
    }

    QSqlQuery query(db);
    if (!query.prepare("INSERT OR IGNORE INTO words (word, definition, hint) VALUES (:word, :definition, :hint);")) 
    {
        Logger::getInstance().log(Logger::LogLevel::Error, "Failed to prepare insert query", query.lastError().text());
        db.rollback();
        return false;
    }

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty())
        {
            query.bindValue(":word", line);
            query.bindValue(":definition", QByteArray());
            query.bindValue(":hint", QByteArray());

            if (!query.exec())
            {
                Logger::getInstance().log(Logger::LogLevel::Warning, QString("Failed to insert word '%1'").arg(line), query.lastError().text());
            }
        }
    }

    if (!db.commit())
    {
        Logger::getInstance().log(Logger::LogLevel::Error, "Failed to commit database transaction", db.lastError().text());
        db.rollback();
        return false;
    }
    return true;
}