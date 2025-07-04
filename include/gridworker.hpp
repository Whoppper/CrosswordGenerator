#ifndef GRIDWORKER_HPP
#define GRIDWORKER_HPP

#include <QObject>
#include <QSize>
#include <QString>
#include <QSharedPointer>

#include "wordtree.hpp"

class DatabaseManager;

class GridWorker : public QObject
{
    Q_OBJECT
public:
    explicit GridWorker(const QSize& workerGridSize, const QString& dbFilePath, int workerDurationMs, QSharedPointer<WordTree> sharedWordTree, QObject *parent = nullptr);

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
};

#endif // GRIDWORKER_HPP