#include "logger.hpp"
#include <QDebug>


Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
{
    loadSettings();

    QDir dir(logDirectory);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    openLogFile();
    connect(this, &Logger::logMessage, this, &Logger::writeLogMessage);
}

Logger::~Logger()
{
    if (logFile.isOpen())
    {
        logFile.close();
    }
}

void Logger::log(LogLevel level, const QString &message, const QString &username)
{
    emit logMessage(level, message, username);
}

void Logger::openLogFile()
{
    if (logFile.isOpen())
    {
        logFile.close();
    }

    logFile.setFileName(getLogFileName());
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        return ;
    }

    logStream.setDevice(&logFile);
}

void Logger::rotateLogFile()
{
    logFile.close();

    QString oldFileName = getLogFileName();
    QString newFileName = oldFileName + "." + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");

    if (QFile::exists(newFileName))
    {
        QFile::remove(newFileName);
    }

    if (QFile::rename(oldFileName, newFileName))
    {
        openLogFile();
    }
}

QString Logger::getLogFileName()
{
    return logDirectory + "/log_" + "_" + QDate::currentDate().toString("yyyyMMdd") + ".log";
}

void Logger::writeLogMessage(LogLevel level, const QString &message, const QString &username)
{
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

    QString logMessage = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + " [" + levelStr + "] " + username + ": "+ message;
    logStream << logMessage << Qt::endl;
    logStream.flush();

    if (logFile.size() > maxFileSize)
    {
        rotateLogFile();
    }
}

void Logger::loadSettings()
{
    QSettings settings(":/data/config.ini", QSettings::IniFormat);
    logDirectory = settings.value("Log/logDirectory", "./logs").toString();
    maxFileSize = settings.value("Logg/maxFileSizeMo", 10 * 1024 * 1024).toLongLong();
}