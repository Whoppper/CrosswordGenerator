#include "databasegenerationinterface.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QTextEdit> 
#include <QCloseEvent> 

#include "databasemanager.hpp"
#include "logger.hpp"

DatabaseGenerationInterface::DatabaseGenerationInterface(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Génération de la Base de Données");
    setFixedSize(800, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(new QLabel("Fichier texte d'entrée :"));
    inputFilePathEdit = new QLineEdit(this);
    inputFilePathEdit->setPlaceholderText("Sélectionnez le fichier texte source des mots...");
    inputLayout->addWidget(inputFilePathEdit);
    selectInputFileButton = new QPushButton("Parcourir...", this);
    inputLayout->addWidget(selectInputFileButton);
    mainLayout->addLayout(inputLayout);

    QHBoxLayout *outputLayout = new QHBoxLayout();
    outputLayout->addWidget(new QLabel("Dossier de sortie (DB) :"));
    outputFolderPathEdit = new QLineEdit(this);
    outputFolderPathEdit->setPlaceholderText("Sélectionnez le dossier où créer la DB...");
    outputLayout->addWidget(outputFolderPathEdit);
    selectOutputFolderButton = new QPushButton("Parcourir...", this);
    outputLayout->addWidget(selectOutputFolderButton);
    mainLayout->addLayout(outputLayout);

    generateDatabaseButton = new QPushButton("Lancer la Génération de la DB", this);
    generateDatabaseButton->setFixedHeight(50);
    mainLayout->addWidget(generateDatabaseButton);

    connect(selectInputFileButton, &QPushButton::clicked, this, &DatabaseGenerationInterface::onSelectInputFileClicked);
    connect(selectOutputFolderButton, &QPushButton::clicked, this, &DatabaseGenerationInterface::onSelectOutputFolderClicked);
    connect(generateDatabaseButton, &QPushButton::clicked, this, &DatabaseGenerationInterface::onGenerateDatabaseClicked);
}

DatabaseGenerationInterface::~DatabaseGenerationInterface()
{

}

void DatabaseGenerationInterface::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    QWidget::closeEvent(event);
}

void DatabaseGenerationInterface::onSelectInputFileClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Sélectionner le fichier texte d'entrée", "", "Fichiers texte (*.txt);;Tous les fichiers (*)");
    if (!filePath.isEmpty())
    {
        inputFilePathEdit->setText(filePath);
    }
}

void DatabaseGenerationInterface::onSelectOutputFolderClicked()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Sélectionner le dossier de sortie pour la DB");
    if (!folderPath.isEmpty())
    {
        outputFolderPathEdit->setText(folderPath);
    }
}

void DatabaseGenerationInterface::onGenerateDatabaseClicked()
{

    QString inputFilePath = inputFilePathEdit->text();
    QString outputFolderPath = outputFolderPathEdit->text();

    if (inputFilePath.isEmpty() || outputFolderPath.isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "Veuillez spécifier tous les chemins (fichier d'entrée, dossier de sortie).");
        return;
    }


    // QProcess *process = new QProcess(this);
    // // Connecter les signaux standardOutput et standardError pour voir les logs du script Python
    // connect(process, &QProcess::readyReadStandardOutput, [this, process](){
    //     logMessage("Python STDOUT: " + process->readAllStandardOutput());
    // });
    // connect(process, &QProcess::readyReadStandardError, [this, process](){
    //     logMessage("Python STDERR: " + process->readAllStandardError());
    // });
    // connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
    //         [this, process](int exitCode, QProcess::ExitStatus exitStatus){
    //     if (exitStatus == QProcess::NormalExit && exitCode == 0) {
    //         logMessage("Script Python exécuté avec succès.");
    //         // Ici, tu peux appeler la logique C++ de remplissage de la DB
    //         // Ou si le script Python remplit la DB, tu passes à la vérification
    //         generateDatabase(); // Appelle la logique C++ de gestion de la DB
    //     } else {
    //         logMessage(QString("Erreur lors de l'exécution du script Python. Code : %1").arg(exitCode));
    //         QMessageBox::critical(this, "Erreur Python", "Le script Python n'a pas pu être exécuté correctement.");
    //     }
    //     process->deleteLater();
    // });

    // // Exemple d'exécution du script Python (adapte les arguments)
    // // Par exemple, si ton script prend le fichier d'entrée et le dossier de sortie en arguments
    // process->start("python", QStringList() << pythonScriptPath << inputFilePath << outputFolderPath);
    // if (!process->waitForStarted()) {
    //     logMessage("Erreur: Impossible de démarrer le processus Python.");
    //     QMessageBox::critical(this, "Erreur", "Impossible de démarrer le processus Python. Assurez-vous que Python est dans le PATH.");
    //     delete process;
    //     return;
    // }

    generateDatabase(); 
}


void DatabaseGenerationInterface::generateDatabase()
{
    QString inputFilePath = inputFilePathEdit->text();
    QString DBPath = outputFolderPathEdit->text() + "/dictionary.db";
    DatabaseManager mainDbManager("MainThreadConnection", DBPath);

    if (!mainDbManager.openDatabase())
    {
        Logger::getInstance().log(Logger::Critical, "Main thread: Échec critique de l'ouverture de la base de données principale. Arrêt.");
        return;
    }
    Logger::getInstance().log(Logger::Info, "Base de données ouverte avec succès.");

    if (!mainDbManager.createTables())
    {
        Logger::getInstance().log(Logger::Critical, "Main thread: Échec critique de la création des tables. Arrêt.");
        mainDbManager.closeDatabase();
        return;
    }
    Logger::getInstance().log(Logger::Info, "Tables vérifiées/créées.");


    if (mainDbManager.isEmpty())
    {
        Logger::getInstance().log(Logger::Info, "Main thread: La base de données est vide, démarrage du remplissage.");
        if (!mainDbManager.fillDB(inputFilePath))
        {
            Logger::getInstance().log(Logger::Critical, "Main thread: Échec critique du remplissage de la base de données. Arrêt.");
            mainDbManager.closeDatabase();
            return;
        }
        Logger::getInstance().log(Logger::Info, "Main thread: Base de données remplie avec succès.");
    }
    else
    {
        Logger::getInstance().log(Logger::Info, "Main thread: La base de données contient déjà des données. Saut du remplissage.");
    }

    mainDbManager.closeDatabase();
    Logger::getInstance().log(Logger::Info, "Base de données fermée. Processus terminé.");
}