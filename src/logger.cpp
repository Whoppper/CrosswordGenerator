#include "logger.hpp"

#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QSettings>
#include <QMutexLocker>
#include <QThread>

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
{
    loadSettings();

    #ifdef QT_NO_DEBUG
        minimumLogLevel = Info;
    #else
        minimumLogLevel = Debug;
    #endif

    QDir dir(logDirectory);
    if (!dir.exists())
    {
        dir.mkpath(logDirectory);
    }

    openLogFile();
    connect(this, &Logger::logMessage, this, &Logger::writeLogMessage, Qt::QueuedConnection);
    #ifdef QT_NO_DEBUG
        log(Info, "Logger initialisé en mode RELEASE. Niveau de log minimum : INFO.");
    #else
        log(Info, "Logger initialisé en mode DEBUG. Niveau de log minimum : DEBUG.");
    #endif
}

Logger::~Logger()
{
    QMutexLocker locker(&mutex);
    if (logFile.isOpen())
    {
        logStream.flush();
        logFile.close();
    }
}

void Logger::log(LogLevel level, const QString &message)
{
    if (level >= minimumLogLevel)
    {
        emit logMessage(level, message);
    }
}

void Logger::openLogFile()
{
    QMutexLocker locker(&mutex);
    if (logFile.isOpen())
    {
        logStream.flush();
        logFile.close();
    }

    logFile.setFileName(getLogFileName(0));
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        return ;
    }

    logStream.setDevice(&logFile);
    logStream.setCodec("UTF-8");
}

void Logger::rotateLogFile()
{
    QMutexLocker locker(&mutex);

    logStream.flush();
    logFile.close();

    QString oldestFileName = getLogFileName(maxLogFiles - 1);
    if (QFile::exists(oldestFileName))
    {
        QFile::remove(oldestFileName);
    }
    for (int i = maxLogFiles - 2; i >= 0; --i) // Commence par l'avant-dernier fichier archivé
    {
        QString src = getLogFileName(i);
        QString dest = getLogFileName(i + 1);

        if (QFile::exists(src))
        {
            if (QFile::exists(dest))
            {
                QFile::remove(dest);
            }
            QFile::rename(src, dest);
        }
    }
    openLogFile();
}

QString Logger::getLogFileName(int index)
{
    return logDirectory + "/log_" +  QString::number(index) + ".log";
}

void Logger::writeLogMessage(LogLevel level, const QString &message)
{
    QMutexLocker locker(&mutex);
    QString levelStr;
    switch (level) {
    case Debug:
        levelStr = "DEBUG";
        break;
    case Info:
        levelStr = "INFO";
        break;
    case Warning:
        levelStr = "WARNING";
        break;
    case Error:
        levelStr = "ERROR";
        break;
    case Critical:
        levelStr = "CRITICAL";
        break;
    }
    QString threadId = QString::number((qintptr)QThread::currentThreadId());
    QString logMessage = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") +
                         " [" + levelStr + "] " +
                         "[" + threadId + "] " + 
                         ": " + message;
    logStream << logMessage << Qt::endl;
    logFile.flush();
    if (logFile.size() > logFileSizeLimit)
    {
        rotateLogFile();
    }

}

void Logger::loadSettings()
{
    QSettings settings(":/data/config.ini", QSettings::IniFormat);
    logDirectory = settings.value("Log/logDirectory", "./logs").toString();
    maxLogFiles = settings.value("Log/maxLogFiles", 5).toInt();
    logFileSizeLimit = settings.value("Log/logFileSizeLimit", 50 * 1024 * 1024).toLongLong();
    if (maxLogFiles < 1)
    {
        maxLogFiles = 1;
    }
}