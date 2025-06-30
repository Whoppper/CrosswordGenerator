#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include <QString>
#include <QSqlError>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(const QString& connectionName, const QString& dbPath, QObject *parent = nullptr);
    ~DatabaseManager() override;

    bool openDatabase();
    void closeDatabase();
    bool isEmpty();

    void fillWordsList(QVector<QString> &words);
    QSqlError lastError() const;

    // à appeler uniquement dans le thread principal
    bool fillDB();
    bool createTables();
    QPair<QString, QString> getWordDetails(const QString& word);

private:
    QSqlDatabase m_db;
    QString m_connectionName;
    QString m_dbPath;

    Q_DISABLE_COPY(DatabaseManager)
};

#endif // DATABASEMANAGER_HPP