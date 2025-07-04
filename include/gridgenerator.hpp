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
    void startGenerationPool();
    int getNbSuccess() const ;
    int getNbFail() const ;

signals:

    void allGenerationsFinished();
    void generationProgress(int success, int fail);

private slots:

    void onWorkerFinished(bool success);
    void onPoolTimeout();

private:

    void launchNewWorker();
    void stopAllActiveWorkers();

    int maxDurationMs;
    int nbWorkers;
    int workerMaxDurationMs;
    QString dbPath;
    QSize gridSize;

    QTimer poolTimer;
    QList<QPair<QThread*, GridWorker*>> runningWorkerPairs;
    int nbSuccess = 0;
    int nbFail = 0;
    QSharedPointer<WordTree> wordTree;
    DatabaseManager* dbManagerForTree;
};

#endif // GRIDGENERATOR_HPP