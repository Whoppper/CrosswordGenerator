#include "gridworker.hpp"
#include "logger.hpp"
#include <QDateTime>
#include <QThread>

GridWorker::GridWorker(QObject *parent)
    : QObject(parent)
{

}

void GridWorker::doWork()
{
    QString result = "failed";
    Logger::getInstance().log(Logger::Info, QString("Worker (Thread ID: %1) démarré.")
                                .arg((qintptr)QThread::currentThreadId()));

   

                          
    Logger::getInstance().log(Logger::Info, QString("Worker (Thread ID: %1) terminé : %2")
                                .arg((qintptr)QThread::currentThreadId())
                                .arg(result));

    //emit gridGenerationFinished(generationSuccessful, result);
}