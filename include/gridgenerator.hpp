#ifndef GRIDGENERATOR_HPP
#define GRIDGENERATOR_HPP

#include <QObject>
#include <QSize>
#include <QList>
#include <QPair>
#include <QTimer>
#include <QVector>

#include "gridworker.hpp"
#include "wordtree.hpp"
#include "databasemanager.hpp"

class GridGenerator : public QObject
{
    Q_OBJECT
public:

    explicit GridGenerator(QObject *parent = nullptr);
    ~GridGenerator();
    void startGenerationPool();
    int getNbSuccess() const ;
    int getNbFail() const ;

    void setGenerationParameters(
        int cols,
        int rows,
        const QString& dbPath,
        int poolDurationMs,
        int workerDurationMs,
        int numWorkers,
        const QString& outputDir,
        const QString& solvingAlgo, 
        const QString& wordSelectionHeuristic
    );

signals:

    void allGenerationsFinished();
    void generationProgress(int success, int fail);
    void totalGenerationProgress(int current);

public slots:
    void stopAllActiveWorkers();

private slots:

    void onWorkerFinished(bool success);
    void onPoolTimeout();

private:

    void launchNewWorker();
    

    QSize gridSize;
    int workerMaxDurationMs;
    int maxDurationMs;
    int nbWorkers;
    QString dbPath;
    QString outputDirectory;
    QString solvingAlgoName;
    QString wordSelectionHeuristicName;


    QTimer poolTimer;
    QList<QPair<QThread*, GridWorker*>> runningWorkerPairs;
    int nbSuccess = 0;
    int nbFail = 0;
    QSharedPointer<WordTree> wordTree;
    DatabaseManager* dbManagerForTree;
};

#endif // GRIDGENERATOR_HPP