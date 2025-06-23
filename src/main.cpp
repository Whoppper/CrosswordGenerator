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


void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    if (msg.contains("QSocketNotifier"))
    {
        return;
    }
    qDebug() << msg;
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageHandler);

    QApplication app(argc, argv);

    QFile file(":/data/styles.qss");  
    if (file.open(QFile::ReadOnly))
    {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        app.setStyleSheet(styleSheet); 
    }

    DatabaseManager &db = DatabaseManager::getInstance();
    if (!db.initializeDatabase("../dictionary.db"))
    {
        return -1;
    }

    CrosswordManager &crosswordManager = CrosswordManager::getInstance();
    crosswordManager.createGrid(10, 10);
    crosswordManager.displayGrid();

    app.exec();
    return 0;
}
