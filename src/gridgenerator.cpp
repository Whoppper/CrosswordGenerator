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

    QSettings settings(":/data/config.ini", QSettings::IniFormat);
    gridSize.setWidth(settings.value("Crossword/gridCols", 10).toInt());
    gridSize.setHeight(settings.value("Crossword/gridRows", 10).toInt());
    workerMaxDurationMs = settings.value("Thread/WorkerMaxDurationMs", 30000).toInt();
    maxDurationMs = settings.value("Thread/TheadPoolMaxDurationMs", 60000).toInt();
    nbWorkers = settings.value("Thread/NbWorkers", 5).toInt();
    dbPath = settings.value("Database/DBPath", "../dictionary.db").toString();

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
    
    GridWorker* worker = new GridWorker(gridSize, dbPath, workerMaxDurationMs, wordTree); // delete par le deleteLater

    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &GridWorker::doWork);
    connect(worker, &GridWorker::gridGenerationFinished, this, &GridGenerator::onWorkerFinished);
    connect(worker, &GridWorker::gridGenerationFinished, thread, &QThread::quit);
    connect(worker, &GridWorker::gridGenerationFinished, worker, &QObject::deleteLater);
    
    connect(this, &QObject::destroyed, thread, &QThread::quit);
    connect(this, &QObject::destroyed, worker, &QObject::deleteLater);

    connect(thread, &QThread::finished, thread, &QObject::deleteLater);


    connect(thread, &QThread::finished, this, [this, thread, worker]() {
        runningWorkerPairs.removeOne({thread, worker});
        Logger::getInstance().log(Logger::Debug, QString("GridGenerator: Paire Thread/Worker (%1/%2) retirée de la liste de suivi.")
                                    .arg((qintptr)thread).arg((qintptr)worker));
    });


    thread->start();
    runningWorkerPairs.append({thread, worker});
    Logger::getInstance().log(Logger::Debug, QString("GridGenerator: Nouveau worker lancé."));
}

void GridGenerator::onWorkerFinished(const GeneratedGridData& data)
{
    generatedGrids.append(data);
    if (data.success == true)
        nbSuccess++;

    Logger::getInstance().log(Logger::Info, QString("GridGenerator: Worker terminé. Succès: %1, Thread ID: %2")
                                .arg(data.success ? "Oui" : "Non")
                                .arg(data.workerThreadId));

    emit generationProgress(nbSuccess);

    Logger::getInstance().log(Logger::Info, QString("GridGenerator: Worker terminé"));

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

int GridGenerator::getNbSuccess() const 
{
    return nbSuccess;
}