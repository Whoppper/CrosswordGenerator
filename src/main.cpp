#include "databasemanager.hpp"
#include "crosswordmanager.hpp"
#include "logger.hpp"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QResource>
#include <QFileInfo>
#include <QSettings>
#include <QTimer>
#include <QMetaType>


void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type);
    Q_UNUSED(context);
    #ifdef QT_NO_DEBUG
        return ;
    #endif
    if (msg.contains("QSocketNotifier"))
    {
        return;
    }
    qDebug() << msg;
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageHandler);
    qRegisterMetaType<Logger::LogLevel>("LogLevel");

    QApplication app(argc, argv);

    QFile file(":/data/styles.qss");  
    if (file.open(QFile::ReadOnly))
    {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        app.setStyleSheet(styleSheet); 
    }

    
    DatabaseManager &db = DatabaseManager::getInstance();
    if (!db.initializeDatabase())
    {
        return -1;
    }

    CrosswordManager &crosswordManager = CrosswordManager::getInstance();

    QTimer::singleShot(0, [&]()
    {
        crosswordManager.createGrid(12, 12);
        crosswordManager.startCrosswordGeneration();
    });

    app.exec();
    return 0;
}
