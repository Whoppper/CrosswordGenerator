#include "databasemanager.hpp"
#include "crosswordmanager.hpp"
#include "logger.hpp"
#include "gridworker.hpp"
#include "gridgenerator.hpp"
#include "mainwindow.hpp"

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

    Logger::getInstance();

    QApplication app(argc, argv);
    QFile file(":/data/styles.qss");  
    if (file.open(QFile::ReadOnly))
    {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        app.setStyleSheet(styleSheet); 
    }

    
    QSettings settings(":/data/config.ini", QSettings::IniFormat);
    QString DBPath = settings.value("Database/DBPath", "../dictionary.db").toString();
    DatabaseManager mainDbManager("MainThreadConnection", DBPath);
    if (!mainDbManager.openDatabase())
    {
        Logger::getInstance().log(Logger::Critical, "Main thread: Échec critique de l'ouverture de la base de données principale. Arrêt.");
        return 1;
    }
    if (!mainDbManager.createTables())
    {
        Logger::getInstance().log(Logger::Critical, "Main thread: Échec critique de la création des tables. Arrêt.");
        mainDbManager.closeDatabase();
        return 1;
    }
    if (mainDbManager.isEmpty())
    {
        Logger::getInstance().log(Logger::Info, "Main thread: La base de données est vide, démarrage du remplissage.");
        if (!mainDbManager.fillDB())
        {
            Logger::getInstance().log(Logger::Critical, "Main thread: Échec critique du remplissage de la base de données. Arrêt.");
            mainDbManager.closeDatabase();
            return 1;
        }
        Logger::getInstance().log(Logger::Info, "Main thread: Base de données remplie avec succès.");
    }
    else
    {
        Logger::getInstance().log(Logger::Info, "Main thread: La base de données contient déjà des données. Saut du remplissage.");
    }
    MainWindow mainWindow;
    mainWindow.show();
    
     return app.exec();
}
