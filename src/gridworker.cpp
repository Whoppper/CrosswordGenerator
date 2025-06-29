#include "gridworker.hpp"
#include "logger.hpp"
#include "crosswordmanager.hpp"
#include "databasemanager.hpp"

#include <QThread>
#include <QDateTime>


GridWorker::GridWorker(const QSize& workerGridSize, const QString& dbPath, int workerDurationMs, QObject *parent)
    : QObject(parent),
      m_gridSize(gridSize),
      m_dbFilePath(dbFilePath),
      m_durationMs(workerDurationMs)
{

}

void GridWorker::doWork()
{

    QString currentThreadId = QString::number((qintptr)QThread::currentThreadId());

    Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Démarré pour générer une grille %2x%3.")
                                .arg(currentThreadId)
                                .arg(m_gridSize.width())
                                .arg(m_gridSize.height()));

    QString dbConnectionName = "WorkerDbConn_" + currentThreadId;
    DatabaseManager dbManager(dbConnectionName, m_dbFilePath, this);


    if (!dbManager.openDatabase())
    {
        Logger::getInstance().log(Logger::Error, QString("GridWorker (Thread ID: %1): Impossible d'ouvrir la base de données. Erreur: %2")
                                    .arg(currentThreadId).arg(dbManager.lastError().text()));
        GeneratedGridData failureData = {m_gridSize, "DB_CONNECTION_FAILED", false, currentThreadId};
        emit gridGenerationFinished(failureData);
        return;
    }
    Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Base de données ouverte avec succès.").arg(currentThreadId));


    CrosswordManager crosswordManager(&dbManager,m_durationMs, this);

    QString generatedContent = crosswordManager.generateGrid(m_gridSize);


    bool generationSuccess = !generatedContent.isEmpty();

    Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Fin de génération pour grille %2x%3. Succès: %4.")
                                .arg(currentThreadId)
                                .arg(m_gridSize.width())
                                .arg(m_gridSize.height())
                                .arg(generationSuccess ? "Oui" : "Non"));

    GeneratedGridData resultData = {m_gridSize, generatedContent, generationSuccess, currentThreadId};
    emit gridGenerationFinished(resultData);

}