#ifndef GRIDWORKER_HPP
#define GRIDWORKER_HPP

#include <QObject>

class GridWorker : public QObject
{
    Q_OBJECT
public:
    explicit GridWorker(const QSize& gridSize, QObject *parent = nullptr);

public slots:
    void doWork();

signals:

    void gridGenerationFinished(bool success, const QString& resultInfo);

private:

};

#endif // GRIDWORKER_HPP