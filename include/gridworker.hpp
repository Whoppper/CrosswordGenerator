#ifndef GRIDWORKER_HPP
#define GRIDWORKER_HPP

#include <QObject>
#include <QSize>
#include <QString>
#include <QVariant>

class DatabaseManager;


struct GeneratedGridData
{
    QSize size;
    QString content;
    bool success;
    QString workerThreadId;
};

Q_DECLARE_METATYPE(GeneratedGridData)

class GridWorker : public QObject
{
    Q_OBJECT
public:
    explicit GridWorker(const QSize& workerGridSize, const QString& dbFilePath, int workerDurationMs, QObject *parent = nullptr);

public slots:

    void doWork();

signals:
    void gridGenerationFinished(const GeneratedGridData& data);

private:
    QSize gridSize;
    QString dbFilePath;
    int durationMs;
    DatabaseManager* dbManager;

    Q_DISABLE_COPY(GridWorker)
};

#endif // GRIDWORKER_HPP