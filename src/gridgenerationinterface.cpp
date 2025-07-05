#include "gridgenerationinterface.hpp"
#include "gridgenerator.hpp"
#include "solvingalgorithmfactory.hpp"
#include "wordselectionfactory.hpp"  

#include <QVBoxLayout>
#include <QThread>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QCloseEvent>
#include <QDebug>

GridGenerationInterface::GridGenerationInterface(QWidget *parent)
    : QWidget(parent),
      gridGenerator(nullptr)
{
    setWindowTitle("Générer des Grilles");
    resize(900, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QFormLayout *configLayout = new QFormLayout();
    configLayout->setRowWrapPolicy(QFormLayout::WrapAllRows); 


    QHBoxLayout *gridSizeLayout = new QHBoxLayout();
    gridColsSpinBox = new QSpinBox(this);
    gridColsSpinBox->setRange(5, 20);
    gridColsSpinBox->setValue(10);
    gridSizeLayout->addWidget(new QLabel("Colonnes:"));
    gridSizeLayout->addWidget(gridColsSpinBox);

    gridRowsSpinBox = new QSpinBox(this);
    gridRowsSpinBox->setRange(5, 20);
    gridRowsSpinBox->setValue(10);
    gridSizeLayout->addWidget(new QLabel("Lignes:"));
    gridSizeLayout->addWidget(gridRowsSpinBox);
    gridSizeLayout->addStretch();

    configLayout->addRow("Taille de la grille:", gridSizeLayout);

    QHBoxLayout *dbPathLayout = new QHBoxLayout();
    dbPathLineEdit = new QLineEdit("../dictionary.db", this);
    browseDbButton = new QPushButton("Parcourir...", this);
    dbPathLayout->addWidget(dbPathLineEdit);
    dbPathLayout->addWidget(browseDbButton);
    configLayout->addRow("Chemin BDD mots:", dbPathLayout);

    QHBoxLayout *outputFolderLayout = new QHBoxLayout();
    outputFolderLineEdit = new QLineEdit("../grids", this);
    browseOutputButton = new QPushButton("Parcourir...", this);
    outputFolderLayout->addWidget(outputFolderLineEdit);
    outputFolderLayout->addWidget(browseOutputButton);
    configLayout->addRow("Dossier de sortie:", outputFolderLayout);

    QHBoxLayout *durationLayout = new QHBoxLayout();
    poolDurationSpinBox = new QSpinBox(this);
    poolDurationSpinBox->setRange(1000, 3600000);
    poolDurationSpinBox->setSuffix(" ms");
    poolDurationSpinBox->setValue(60000);
    poolDurationSpinBox->setSingleStep(5000);
    durationLayout->addWidget(new QLabel("Durée max. pool:"));
    durationLayout->addWidget(poolDurationSpinBox);

    workerDurationSpinBox = new QSpinBox(this);
    workerDurationSpinBox->setRange(1000, 600000);
    workerDurationSpinBox->setSuffix(" ms");
    workerDurationSpinBox->setValue(30000);
    workerDurationSpinBox->setSingleStep(5000);
    durationLayout->addWidget(new QLabel("Durée max. worker:"));
    durationLayout->addWidget(workerDurationSpinBox);
    durationLayout->addStretch();

    configLayout->addRow(durationLayout);

    numWorkersSpinBox = new QSpinBox(this);
    numWorkersSpinBox->setRange(1, QThread::idealThreadCount() * 2);
    if (QThread::idealThreadCount() > 0) {
        numWorkersSpinBox->setValue(QThread::idealThreadCount());
    } else {
        numWorkersSpinBox->setValue(4); 
    }
    configLayout->addRow("Nb. Workers:", numWorkersSpinBox);

    solvingAlgoComboBox = new QComboBox(this);
    configLayout->addRow("Algorithme de résolution:", solvingAlgoComboBox);
    setAlgorithmOptions();

    heuristicComboBox = new QComboBox(this);
    configLayout->addRow("Heuristique d'index:", heuristicComboBox);
    setHeuristicOptions();

    mainLayout->addLayout(configLayout);

    QHBoxLayout *actionButtonsLayout = new QHBoxLayout();
    startButton = new QPushButton("Démarrer Génération", this);
    stopButton = new QPushButton("Arrêter Génération", this);
    stopButton->setEnabled(false); 
    actionButtonsLayout->addWidget(startButton);
    actionButtonsLayout->addWidget(stopButton);
    mainLayout->addLayout(actionButtonsLayout);

    statusLabel = new QLabel("Statut: Prêt", this);
    successCountLabel = new QLabel("Grilles réussies: 0", this);
    failCountLabel = new QLabel("Grilles échouées: 0", this);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(true);

    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(successCountLabel);
    mainLayout->addWidget(failCountLabel);
    mainLayout->addWidget(progressBar);


    connect(browseDbButton, &QPushButton::clicked, this, &GridGenerationInterface::onBrowseDbPathButtonClicked);
    connect(browseOutputButton, &QPushButton::clicked, this, &GridGenerationInterface::onBrowseOutputFolderButtonClicked);
    connect(startButton, &QPushButton::clicked, this, &GridGenerationInterface::onStartGenerationButtonClicked);
    connect(stopButton, &QPushButton::clicked, this, &GridGenerationInterface::onStopGenerationButtonClicked);


    gridGenerator = new GridGenerator(this);


    connect(gridGenerator, &GridGenerator::generationProgress, this, &GridGenerationInterface::onGenerationProgress);
    connect(gridGenerator, &GridGenerator::allGenerationsFinished, this, &GridGenerationInterface::onAllGenerationsFinished);
    connect(gridGenerator, &GridGenerator::totalGenerationProgress, this, &GridGenerationInterface::onTotalGenerationProgress);
}

GridGenerationInterface::~GridGenerationInterface()
{

}


void GridGenerationInterface::setAlgorithmOptions()
{
    QStringList options = SolvingAlgorithmFactory::getAvailableAlgorithms();
    solvingAlgoComboBox->clear();
    solvingAlgoComboBox->addItems(options);
}

void GridGenerationInterface::setHeuristicOptions()
{
    QStringList options = WordSelectionFactory::getAvailableWordSelection();
    heuristicComboBox->clear();
    heuristicComboBox->addItems(options);
}


void GridGenerationInterface::onBrowseDbPathButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Sélectionner la base de données", dbPathLineEdit->text(), "Bases de données (*.db *.sqlite)");
    if (!filePath.isEmpty())
    {
        dbPathLineEdit->setText(filePath);
    }
}

void GridGenerationInterface::onBrowseOutputFolderButtonClicked()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "Sélectionner le dossier de sortie", outputFolderLineEdit->text());
    if (!dirPath.isEmpty())
    {
        outputFolderLineEdit->setText(dirPath);
    }
}

void GridGenerationInterface::onStartGenerationButtonClicked()
{
    int cols = gridColsSpinBox->value();
    int rows = gridRowsSpinBox->value();
    QString dbPath = dbPathLineEdit->text();
    int poolDuration = poolDurationSpinBox->value();
    int workerDuration = workerDurationSpinBox->value();
    int numWorkers = numWorkersSpinBox->value();
    QString outputDir = outputFolderLineEdit->text();
    QString solvingAlgo = solvingAlgoComboBox->currentText();
    QString heuristic = heuristicComboBox->currentText();


    if (dbPath.isEmpty() || outputDir.isEmpty())
    {
        QMessageBox::warning(this, "Paramètres manquants", "Veuillez spécifier le chemin de la base de données et le dossier de sortie.");
        return;
    }

    setControlsEnabled(false);
    startButton->setEnabled(false);
    stopButton->setEnabled(true);

    statusLabel->setText("Statut: Génération en cours...");
    successCountLabel->setText("Grilles réussies: 0");
    failCountLabel->setText("Grilles échouées: 0");
    progressBar->setValue(0);

    gridGenerator->setGenerationParameters(cols, rows, dbPath, poolDuration, workerDuration, numWorkers, outputDir, solvingAlgo, heuristic);
    gridGenerator->startGenerationPool();
}

void GridGenerationInterface::onStopGenerationButtonClicked()
{
    statusLabel->setText("Statut: Arrêt demandé...");
    stopButton->setEnabled(false);

    gridGenerator->stopAllActiveWorkers();
}

void GridGenerationInterface::onGenerationProgress(int successCount, int failCount)
{
    successCountLabel->setText(QString("Grilles réussies: %1").arg(successCount));
    failCountLabel->setText(QString("Grilles échouées: %1").arg(failCount));
}

void GridGenerationInterface::onTotalGenerationProgress(int current)
{
    if (current > 0)
    {
        progressBar->setValue(current);
    }
}

void GridGenerationInterface::onAllGenerationsFinished()
{
    statusLabel->setText("Statut: Terminé !");
    setControlsEnabled(true);
    startButton->setEnabled(true);
    stopButton->setEnabled(false); 
    
}

void GridGenerationInterface::closeEvent(QCloseEvent *event)
{
    if (startButton->isEnabled() == false)
    {
        QMessageBox::information(this, "Génération en cours", "Veuillez arrêter la génération avant de fermer cette fenêtre.");
        event->ignore();
        return;
    }
    emit windowClosed();
    QWidget::closeEvent(event);
}

void GridGenerationInterface::setControlsEnabled(bool enable)
{
    gridColsSpinBox->setEnabled(enable);
    gridRowsSpinBox->setEnabled(enable);
    dbPathLineEdit->setEnabled(enable);
    browseDbButton->setEnabled(enable);
    poolDurationSpinBox->setEnabled(enable);
    workerDurationSpinBox->setEnabled(enable);
    numWorkersSpinBox->setEnabled(enable);
    outputFolderLineEdit->setEnabled(enable);
    browseOutputButton->setEnabled(enable);
    solvingAlgoComboBox->setEnabled(enable);
    heuristicComboBox->setEnabled(enable);
}