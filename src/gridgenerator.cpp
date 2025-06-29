#include "gridgenerator.hpp"
#include "gridworker.hpp"
#include "logger.hpp" 

#include <QThread>
#include <QSettings>

GridGenerator::GridGenerator(QObject *parent)
    : QObject(parent)
{
    
    connect(&poolTimer, &QTimer::timeout, this, &GridGenerator::onPoolTimeout);

    gridSize.setWidth(settings.value("Crossword/gridCols", 10).toInt());
    gridSize.setHeight(settings.value("Crossword/gridRows", 10).toInt());
    workerMaxDurationMs = settings.value("Thread/WorkerMaxDurationMs", 30000).toInt();
    maxDurationMs = settings.value("Thread/ThreadPoolMaxDurationMs", 60000).toInt();
    nbWorkers = settings.value("Thread/NbWorkers", 5).toInt();
    dbPath = settings.value("Database/DBPath", "../dictionary.db").toString();
}


void GridGenerator::startGenerationPool()
{
    Logger::getInstance().log(Logger::Info, QString("GridGenerator: Démarrage du pool de génération : %1 threads, %2ms max, taille %3x%4.")
                                .arg(nbWorkers)
                                .arg(maxDurationMs)
                                .arg(gridSize.width())
                                .arg(gridSize.height()));

    
    generatedGrids.clear();
    runningWorkerPairs.clear();

    for (int i = 0; i < m_initialWorkersCount; ++i)
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
    QThread* thread = new QThread(this);
    
    GridWorker* worker = new GridWorker(gridSize, dbPath, workerMaxDurationMs);

    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &GridWorker::doWork);
    connect(worker, &GridWorker::gridGenerationFinished, this, &GridGenerator::onWorkerFinished);
    connect(worker, &GridWorker::gridGenerationFinished, thread, &QThread::quit);
    connect(worker, &GridWorker::gridGenerationFinished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    connect(this, &QObject::destroyed, thread, &QThread::quit);
    connect(this, &QObject::destroyed, worker, &QObject::deleteLater);


    thread->start();
    runningWorkerPairs.append({thread, worker});.
    Logger::getInstance().log(Logger::Debug, QString("GridGenerator: Nouveau worker lancé.");
}

void GridGenerator::onWorkerFinished(/*const GeneratedGridData& data*/)
{
    /*generatedGrids.append(data);

    Logger::getInstance().log(Logger::Info, QString("GridGenerator: Worker terminé (grilles générées: %1). Succès: %2, Thread ID: %3")
                                .arg(generatedGrids.size())
                                .arg(data.success ? "Oui" : "Non")
                                .arg(data.workerThreadId));

    emit generationProgress(generatedGrids.size());*/

     Logger::getInstance().log(Logger::Info, QString("GridGenerator: Worker terminé");

     if (poolTimer.isActive())
     { 
        Logger::getInstance().log(Logger::Debug, "GridGenerator: Relance d'un nouveau worker.");
        launchNewWorker();
    }
}


void GridGenerator::onPoolTimeout()
{
    Logger::getInstance().log(Logger::Info, "GridGenerator: Temps imparti pour la génération écoulé. Arrêt du pool.");
    poolTimer.stop(); 

    stopAllActiveWorkers(); 
    emit allGenerationsFinished();
}


void GridGenerator::stopAllActiveWorkers()
{
    Logger::getInstance().log(Logger::Debug, QString("GridGenerator: Tentative d'arrêt des workers."));
    for (auto& pair : runningWorkerPairs)
    {
        QThread* thread = pair.first;
        if (thread->isRunning())
        {
            thread->quit();
        }
    }
    runningWorkerPairs.clear();
}


QVector<GeneratedGridData> GridGenerator::getGeneratedGrids() const
{
    return generatedGrids;
}