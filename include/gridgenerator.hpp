#ifndef GRIDGENERATOR_HPP
#define GRIDGENERATOR_HPP

#include <QObject>
#include <QSize>
#include <QList>
#include <QPair>
#include <QTimer>
#include <QVector>

#include "gridworker.hpp"

class GridGenerator : public QObject
{
    Q_OBJECT
public:

    explicit GridGenerator(QObject *parent = nullptr);
    void startGenerationPool();
    QVector<GeneratedGridData> getGeneratedGrids() const;
    int getNbSuccess() const ;

signals:

    void allGenerationsFinished();
    void generationProgress(int completedCount);

private slots:

    void onWorkerFinished(const GeneratedGridData& data);
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
    QVector<GeneratedGridData> generatedGrids;
    int nbSuccess = 0;
};

#endif // GRIDGENERATOR_HPP