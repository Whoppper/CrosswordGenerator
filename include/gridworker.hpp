#ifndef GRIDWORKER_HPP
#define GRIDWORKER_HPP

#include <QObject>
#include <QSize>
#include <QString>
#include <QVariant>


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
    explicit GridWorker(const QSize& gridSize, const QString& dbFilePath, int workerDurationMs, QObject *parent = nullptr);

public slots:

    void doWork();

signals:
    void gridGenerationFinished(const GeneratedGridData& data);

private:
    QSize m_gridSize;
    QString m_dbFilePath;
    int m_durationMs;
};

#endif // GRIDWORKER_HPP