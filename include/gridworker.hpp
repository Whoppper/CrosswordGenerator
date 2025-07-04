#ifndef GRIDWORKER_HPP
#define GRIDWORKER_HPP

#include <QObject>
#include <QSize>
#include <QString>
#include <QSharedPointer>
#include "wordtree.hpp"


class DatabaseManager;
class CrosswordManager;

class GridWorker : public QObject
{
    Q_OBJECT
public:
    explicit GridWorker(const QSize& workerGridSize,
                        const QString& dbPath,
                        int workerDurationMs,
                        QSharedPointer<WordTree> sharedWordTree,
                        const QString& outputDir,
                        const QString& solvingAlgo,
                        const QString& wordSelectionHeuristic,
                        QObject *parent = nullptr);

    void requestAlgorithmStop();

public slots:
    void doWork();

signals:
    void gridGenerationFinished(bool success);

private:
    QSize gridSize;
    QString dbFilePath;
    int durationMs;
    DatabaseManager* dbManager; //delete by parent
    QSharedPointer<WordTree> wordTree;
    Q_DISABLE_COPY(GridWorker)
    CrosswordManager *crosswordManager; // delete by parent
    QString outputBaseDirectory;
    QString solvingAlgoName;
    QString wordSelectionHeuristicName;
};

#endif // GRIDWORKER_HPP