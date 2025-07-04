#include "gridgenerator.hpp"
#include "gridworker.hpp"
#include "logger.hpp" 

#include <QThread>
#include <QSettings>
#include <QSharedPointer>

GridGenerator::GridGenerator(QObject *parent)
    : QObject(parent)
{
    
    connect(&poolTimer, &QTimer::timeout, this, &GridGenerator::onPoolTimeout);
}


void GridGenerator::startGenerationPool()
{
    dbManagerForTree = new DatabaseManager("TreeDbConnection", dbPath, this);
    if (!dbManagerForTree->openDatabase())
    {
        Logger::getInstance().log(Logger::Error, "GridGenerator: Impossible d'ouvrir la base de données pour l'arbre de mots. Erreur: " + dbManagerForTree->lastError().text());
        return ;
    }
    else
    {
        Logger::getInstance().log(Logger::Info, "GridGenerator: Base de données ouverte pour l'arbre de mots. Remplissage de l'arbre...");
        QVector<QString> allWords;
        dbManagerForTree->fillWordsList(allWords);
        wordTree = QSharedPointer<WordTree>::create();
        for (const QString &word : allWords)
        {
            wordTree->insert(word);
        }
        dbManagerForTree->closeDatabase();
        Logger::getInstance().log(Logger::Info, "GridGenerator: Arbre de mots rempli et base de données fermée.");
    }


    Logger::getInstance().log(Logger::Info, QString("GridGenerator: Démarrage du pool de génération : %1 threads, %2ms max, taille %3x%4.")
                                .arg(nbWorkers)
                                .arg(maxDurationMs)
                                .arg(gridSize.width())
                                .arg(gridSize.height()));

    
    runningWorkerPairs.clear();

    for (int i = 0; i < nbWorkers; ++i)
    {
        launchNewWorker();
    }

    if (maxDurationMs > 0)
    {
        poolTimer.start(maxDurationMs);
    }
}


void GridGenerator::launchNewWorker()
{
    QThread* thread = new QThread(); // delete par le deleteLater
    
    GridWorker* worker = new GridWorker(gridSize, dbPath, workerMaxDurationMs, wordTree, outputDirectory,
                                        solvingAlgoName, wordSelectionHeuristicName); // delete par le deleteLater

    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &GridWorker::doWork);
    connect(worker, &GridWorker::gridGenerationFinished, this, &GridGenerator::onWorkerFinished);
    connect(worker, &GridWorker::gridGenerationFinished, thread, &QThread::quit);
    connect(worker, &GridWorker::gridGenerationFinished, worker, &QObject::deleteLater);
    
    connect(this, &QObject::destroyed, thread, &QThread::quit);
    connect(this, &QObject::destroyed, worker, &QObject::deleteLater);

    connect(thread, &QThread::finished, thread, &QObject::deleteLater);


    connect(thread, &QThread::finished, this, [this, thread, worker]()
    {
        runningWorkerPairs.removeOne({thread, worker});
        Logger::getInstance().log(Logger::Debug, QString("GridGenerator: Paire Thread/Worker (%1/%2) retirée de la liste de suivi.")
                                    .arg((qintptr)thread).arg((qintptr)worker));

        if (!poolTimer.isActive() && runningWorkerPairs.isEmpty())
        {
            Logger::getInstance().log(Logger::Info, "GridGenerator: Tous les workers se sont arrêtés. Fin de toutes les générations.");
            emit totalGenerationProgress(100);
            emit allGenerationsFinished();
        }   
    });

    thread->start();
    runningWorkerPairs.append({thread, worker});
    Logger::getInstance().log(Logger::Debug, QString("GridGenerator: Nouveau worker lancé."));
}

void GridGenerator::onWorkerFinished(bool success)
{

    if (success)
        nbSuccess++;
    else
        nbFail++;

    Logger::getInstance().log(Logger::Info, QString("GridGenerator: Worker terminé. Succès: %1").arg(success));
    Logger::getInstance().log(Logger::Info, QString("Total: Succès: %1 . Echecs: %2").arg(nbSuccess).arg(nbFail));
    emit generationProgress(nbSuccess, nbFail);
    
    if (poolTimer.isActive())
    { 
        emit totalGenerationProgress(100 - poolTimer.remainingTime() * 100 / maxDurationMs);
        Logger::getInstance().log(Logger::Debug, "GridGenerator: Relance d'un nouveau worker.");
        launchNewWorker();
    }
}


void GridGenerator::onPoolTimeout()
{
    stopAllActiveWorkers(); 
}


void GridGenerator::stopAllActiveWorkers()
{
    Logger::getInstance().log(Logger::Info, "GridGenerator: Temps imparti pour la génération écoulé. Arrêt du pool.");
    poolTimer.stop(); 
    Logger::getInstance().log(Logger::Debug, QString("GridGenerator: Tentative d'arrêt des workers."));
    for (auto& pair : runningWorkerPairs)
    {
        QThread* thread = pair.first;
        GridWorker* worker = pair.second;
        if (thread->isRunning())
        {
            worker->requestAlgorithmStop();
            thread->quit();
        }
    }
}

int GridGenerator::getNbSuccess() const 
{
    return nbSuccess;
}

int GridGenerator::getNbFail() const 
{
    return nbFail;
}

void GridGenerator::setGenerationParameters(
    int cols,
    int rows,
    const QString& newDbPath,
    int poolDuration,
    int workerDuration,
    int numWorkers,
    const QString& outputDir,
    const QString& solvingAlgo,
    const QString& wordSelectionHeuristic
)
{
    gridSize.setWidth(cols);
    gridSize.setHeight(rows);
    dbPath = newDbPath;
    maxDurationMs = poolDuration;
    workerMaxDurationMs = workerDuration;
    nbWorkers = numWorkers;
    outputDirectory = outputDir;

    solvingAlgoName = solvingAlgo;
    wordSelectionHeuristicName = wordSelectionHeuristic;
}

GridGenerator::~GridGenerator()
{
    stopAllActiveWorkers();
}