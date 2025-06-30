#include "databasemanager.hpp"
#include "crosswordmanager.hpp"
#include "logger.hpp"
#include "gridworker.hpp"
#include "gridgenerator.hpp"

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
    qRegisterMetaType<GeneratedGridData>("GeneratedGridData");

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

    GridGenerator gridGenerator; 

    //QObject::connect(&gridGenerator, &GridGenerator::allGenerationsFinished, &app, &QCoreApplication::quit);

    QObject::connect(&gridGenerator, &GridGenerator::allGenerationsFinished, [&](){
        int nbsuccess = gridGenerator.getNbSuccess();
        Logger::getInstance().log(Logger::Info, QString("Application terminée. Total de grilles générées collectées : %1").arg(nbsuccess));
        qDebug() << QString("Application terminée. Total de grilles générées collectées : %1").arg(nbsuccess);

        QTimer::singleShot(1000, [&]()
        {
            QCoreApplication::quit();
        });
    });

    QObject::connect(&gridGenerator, &GridGenerator::generationProgress, [&](int completed){
        Logger::getInstance().log(Logger::Info, QString("Progression du pool : %1 grilles générées.").arg(completed));
        qDebug() << QString("Progression du pool : %1 grilles générées.").arg(completed);
    });



    QTimer::singleShot(0, [&]()
    {
        Logger::getInstance().log(Logger::Info, "Démarrage des opérations via QTimer::singleShot.");
        gridGenerator.startGenerationPool();
    });

    app.exec();

    //QVector<GeneratedGridData> finalGrids = gridGenerator.getGeneratedGrids();
    //Logger::getInstance().log(Logger::Info, QString("Application terminée. Total de grilles générées collectées : %1").arg(finalGrids.size()));
    
    
    return 0;
}
