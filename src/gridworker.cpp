#include "gridworker.hpp"
#include "logger.hpp"
#include "crosswordmanager.hpp"
#include "databasemanager.hpp"

#include <QThread>

GridWorker::GridWorker(const QSize& workerGridSize, const QString& dbPath, int workerDurationMs, QObject *parent)
    : QObject(parent),
      gridSize(workerGridSize),
      dbFilePath(dbPath),
      durationMs(workerDurationMs)
{

}

void GridWorker::doWork()
{

    QString currentThreadId = QString::number((qintptr)QThread::currentThreadId());

    Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Démarré pour générer une grille %2x%3.")
                                .arg(currentThreadId)
                                .arg(gridSize.width())
                                .arg(gridSize.height()));

    QString dbConnectionName = "WorkerDbConn_" + currentThreadId;
    dbManager = new DatabaseManager (dbConnectionName, dbFilePath, this);


    if (!dbManager->openDatabase())
    {
        Logger::getInstance().log(Logger::Error, QString("GridWorker (Thread ID: %1): Impossible d'ouvrir la base de données. Erreur: %2")
                                    .arg(currentThreadId).arg(dbManager->lastError().text()));
        GeneratedGridData failureData = {gridSize, "", false, currentThreadId};
        emit gridGenerationFinished(failureData);
        return;
    }
    Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Base de données ouverte avec succès.").arg(currentThreadId));


    QString generatedContent;
    CrosswordManager crosswordManager(dbManager,durationMs, this);
    bool ok = crosswordManager.generateGrid(gridSize.height(), gridSize.height());
    if (ok)
    {
        generatedContent = crosswordManager.startCrosswordGeneration();
    }
    else
    {
         Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Impossible de créer la grille.")
                                .arg(currentThreadId));
    }

    // peut etre pas necessaire mais ça fait pas de mal
    dbManager->closeDatabase();
    Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Base de données fermée explicitement.").arg(currentThreadId));
    
    bool generationSuccess = !generatedContent.isEmpty();

    Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Fin de génération pour grille %2x%3. Succès: %4.")
                                .arg(currentThreadId)
                                .arg(gridSize.width())
                                .arg(gridSize.height())
                                .arg(generationSuccess ? "Oui" : "Non"));

    GeneratedGridData resultData = {gridSize, generatedContent, generationSuccess, currentThreadId};
    emit gridGenerationFinished(resultData);

}