#include "databasemanager.hpp"
#include "logger.hpp"

#include <QRegularExpression>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QThread>

DatabaseManager::DatabaseManager(const QString& connectionName, const QString& dbPath, QObject *parent)
    : QObject(parent),
      m_connectionName(connectionName),
      m_dbPath(dbPath)
{
    if (QSqlDatabase::contains(m_connectionName))
    {
        m_db = QSqlDatabase::database(m_connectionName);
        Logger::getInstance().log(Logger::Debug, QString("DatabaseManager '%1': Connexion existante réutilisée pour DB '%2'.").arg(m_connectionName).arg(m_dbPath));
    }
    else
    {
        m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
        m_db.setDatabaseName(m_dbPath);
        Logger::getInstance().log(Logger::Debug, QString("DatabaseManager '%1': Nouvelle connexion créée pour DB '%2'.").arg(m_connectionName).arg(m_dbPath));
    }
}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
    Logger::getInstance().log(Logger::Debug, QString("DatabaseManager '%1' détruit.").arg(m_connectionName));
}

QSqlError DatabaseManager::lastError() const
{
    return m_db.lastError();
}

void DatabaseManager::closeDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
        Logger::getInstance().log(Logger::Debug, QString("DatabaseManager '%1': DB fermée.").arg(m_connectionName));
    }
    m_db = QSqlDatabase();
    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase::removeDatabase(m_connectionName);
        Logger::getInstance().log(Logger::Debug, QString("DatabaseManager '%1': Connexion DB retirée du pool Qt.").arg(m_connectionName));
    }
}

bool DatabaseManager::openDatabase()
{
    if (!m_db.isOpen() && !m_db.open())
    {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Impossible d'ouvrir la base de données : %2")
                                    .arg(m_connectionName).arg(m_db.lastError().text()));
        return false;
    }
    Logger::getInstance().log(Logger::LogLevel::Info, QString("DatabaseManager '%1': Base de données ouverte avec succès.").arg(m_connectionName));
    return true;
}

bool DatabaseManager::createTables()
{
    if (!m_db.isOpen())
    {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Impossible de créer les tables, connexion fermée.").arg(m_connectionName));
        return false;
    }
    QSqlQuery query(m_db);
    QString createWordsTable =
    "CREATE TABLE IF NOT EXISTS words ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "word TEXT UNIQUE NOT NULL, "
    "definition BLOB, "
    "hint BLOB);";

    if (!query.exec(createWordsTable))
    {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Failed to create words table: %2")
                                    .arg(m_connectionName).arg(query.lastError().text()));
        return false;
    }
    Logger::getInstance().log(Logger::LogLevel::Debug, QString("DatabaseManager '%1': Table 'words' vérifiée/créée.").arg(m_connectionName));
    return true;
}


bool DatabaseManager::isEmpty()
{
    if (!m_db.isOpen()) {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Impossible de vérifier si la DB est vide, connexion fermée.").arg(m_connectionName));
        return true;
    }
    QSqlQuery query(m_db);
    QString countQuery = "SELECT COUNT(*) FROM words;";

    if (!query.exec(countQuery))
    {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Failed to execute count query for words table: %2")
                                    .arg(m_connectionName).arg(query.lastError().text()));
        return true;
    }
    if (query.next())
    {
        int rowCount = query.value(0).toInt();
        return rowCount == 0;
    }
    return true;
}

bool DatabaseManager::fillDB(const QString &filePath)
{
    if (!m_db.isOpen())
    {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Impossible de remplir la DB, connexion fermée.").arg(m_connectionName));
        return false;
    }
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Failed to open words file 'dicoClean.txt'.").arg(m_connectionName));
        return false;
    }

    QTextStream in(&file);

    if (!m_db.transaction())
    {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Failed to start database transaction: %2").arg(m_connectionName).arg(m_db.lastError().text()));
        return false;
    }

    QSqlQuery query(m_db);
    if (!query.prepare("INSERT OR IGNORE INTO words (word, definition, hint) VALUES (:word, :definition, :hint);"))
    {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Failed to prepare insert query: %2").arg(m_connectionName).arg(query.lastError().text()));
        m_db.rollback();
        return false;
    }

    int insertedCount = 0;
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
                Logger::getInstance().log(Logger::LogLevel::Warning, QString("DatabaseManager '%1': Failed to insert word '%2': %3")
                                            .arg(m_connectionName).arg(line).arg(query.lastError().text()));
            }
            else
            {
                insertedCount++;
            }
        }
    }

    if (!m_db.commit())
    {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Failed to commit database transaction: %2").arg(m_connectionName).arg(m_db.lastError().text()));
        m_db.rollback();
        return false;
    }
    Logger::getInstance().log(Logger::LogLevel::Info, QString("DatabaseManager '%1': %2 mots insérés/ignorés.").arg(m_connectionName).arg(insertedCount));
    return true;
}

void DatabaseManager::fillWordsList(QVector<QString> &words)
{
    if (!m_db.isOpen())
    {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Database is not open for filling word list.").arg(m_connectionName));
        return;
    }
    QString queryString = "SELECT word FROM words";

    QSqlQuery query(m_db);
    if (!query.exec(queryString))
    {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Failed to execute query for filling word list: %2")
                                    .arg(m_connectionName).arg(query.lastError().text()));
        return;
    }
    while (query.next())
    {
        QString word = query.value(0).toString();
        words.push_back(word);
    }
}

QPair<QString, QString> DatabaseManager::getWordDetails(const QString& word)
{
    QPair<QString, QString> details;

    if (!m_db.isOpen())
    {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Impossible de récupérer les détails du mot '%2', la base de données n'est pas ouverte.")
                                    .arg(m_connectionName).arg(word));
        return details;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT definition, hint FROM words WHERE word = :word");
    query.bindValue(":word", word);

    if (!query.exec())
    {
        Logger::getInstance().log(Logger::LogLevel::Error, QString("DatabaseManager '%1': Échec de l'exécution de la requête pour les détails du mot '%2': %3")
                                    .arg(m_connectionName).arg(word).arg(query.lastError().text()));
        return details;
    }

    if (query.next())
    {
        details.first = query.value("definition").toString();
        details.second = query.value("hint").toString();
    }
    else
    {
        Logger::getInstance().log(Logger::LogLevel::Debug, QString("DatabaseManager '%1': Mot '%2' non trouvé dans la base de données.")
                                    .arg(m_connectionName).arg(word));
    }
    return details;
}