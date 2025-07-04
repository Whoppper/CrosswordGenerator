#include "gridworker.hpp"
#include "logger.hpp"
#include "crosswordmanager.hpp"
#include "databasemanager.hpp"

#include <QThread>
#include <QFile> 
#include <QDir> 
#include <QDateTime> 

GridWorker::GridWorker(const QSize& workerGridSize,
                       const QString& dbPath,
                       int workerDurationMs,
                       QSharedPointer<WordTree> sharedWordTree,
                       const QString& outputDir,
                       const QString& solvingAlgo,
                       const QString& wordSelectionHeuristic,
                       QObject *parent)
    : QObject(parent),
      gridSize(workerGridSize),
      dbFilePath(dbPath),
      durationMs(workerDurationMs),
      dbManager(nullptr), 
      wordTree(sharedWordTree),
      outputBaseDirectory(outputDir),
      solvingAlgoName(solvingAlgo),
      wordSelectionHeuristicName(wordSelectionHeuristic)
{
    if (!wordTree)
    {
        Logger::getInstance().log(Logger::Critical, "GridWorker créé sans WordsTree valide !");
    }
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
        emit gridGenerationFinished(false);
        return;
    }
    Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Base de données ouverte avec succès.").arg(currentThreadId));


    QString generatedContent;
    crosswordManager = new CrosswordManager(dbManager,wordTree, this);
    crosswordManager->setWordsSelectionStrategy(wordSelectionHeuristicName);
    crosswordManager->setSolvingAlgorithmStrategy(solvingAlgoName, durationMs);

    bool ok = crosswordManager->generateGrid(gridSize.height(), gridSize.height());
    if (ok)
    {
        generatedContent = crosswordManager->startCrosswordGeneration();
    }
    else
    {
         Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Impossible de créer la grille.")
                                .arg(currentThreadId));
    }

    // peut etre pas necessaire mais ça fait pas de mal
    dbManager->closeDatabase();
    Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Base de données fermée explicitement.").arg(currentThreadId));
    

    Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Fin de génération pour grille %2x%3. Succès: %4.")
                                .arg(currentThreadId)
                                .arg(gridSize.width())
                                .arg(gridSize.height())
                                .arg(!generatedContent.isEmpty() ? "Oui" : "Non"));

    bool generationSuccessful = false;                                    
    if (!generatedContent.isEmpty())
    {
        QString dirPath = outputBaseDirectory + QString("/%1x%2")
                              .arg(gridSize.width())
                              .arg(gridSize.height()); 

        QDir dir(dirPath);
        if (!dir.exists())
        {
            // en principe ça craint pas meme si cest fait par plusieurs threads
            dir.mkpath(".");
            Logger::getInstance().log(Logger::Debug, QString("GridWorker (Thread ID: %1): Dossier créé : %2").arg(currentThreadId).arg(dirPath));
        }

        QString fileName = QString("%1_%2.json")
                               .arg(currentThreadId)
                               .arg(QDateTime::currentMSecsSinceEpoch());
        QString filePath = dir.filePath(fileName);

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << generatedContent;
            file.close();
            Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Grille sauvegardée dans : %2").arg(currentThreadId).arg(filePath));
            generationSuccessful = true;
        }
        else
        {
            Logger::getInstance().log(Logger::Error, QString("GridWorker (Thread ID: %1): Impossible de sauvegarder la grille dans %2. Erreur: %3")
                                        .arg(currentThreadId).arg(filePath).arg(file.errorString()));
        }
    }
    else
    {
        Logger::getInstance().log(Logger::Info, QString("GridWorker (Thread ID: %1): Pas de sauvegarde de grille (génération échouée ou contenu vide).").arg(currentThreadId));
    }

    emit gridGenerationFinished(generationSuccessful);

}

void GridWorker::requestAlgorithmStop()
{
    if (crosswordManager && crosswordManager->getSolvingAlgorithmStrategy())
        crosswordManager->getSolvingAlgorithmStrategy()->setStopSolving(true);
}