#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class Logger : public QObject
{
    Q_OBJECT

public:
    enum LogLevel {
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };

    
    ~Logger();

    void log(LogLevel level, const QString &message);
    static Logger &getInstance();

signals:
    void logMessage(LogLevel level, const QString &message);

private:
    QString logDirectory;
    qint64 maxLogFiles;
    QFile logFile;
    QTextStream logStream;
    LogLevel minimumLogLevel;

    void openLogFile();
    void rotateLogFile();
    QString getLogFileName(int index);
    Logger();
    void loadSettings();
    void writeLogMessage(LogLevel level, const QString &message);
};

#endif // LOGGER_H