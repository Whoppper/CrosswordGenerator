#include "mainwindow.hpp"
#include "gameinterface.hpp" 
#include "gridgenerationinterface.hpp"

#include <QWidget> 
#include <QVBoxLayout>
#include <QApplication> 

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      gameInterface(nullptr),
      gridGenerationInterface(nullptr)
{
    setWindowTitle("Application Mots Croisés");
    setFixedSize(1000, 800);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20); 

    playButton = new QPushButton("Jouer une partie", centralWidget);
    playButton->setFixedHeight(50); 
    mainLayout->addWidget(playButton);

    generateButton = new QPushButton("Générer des grilles", centralWidget);
    generateButton->setFixedHeight(50);
    mainLayout->addWidget(generateButton);

    connect(playButton, &QPushButton::clicked, this, &MainWindow::onPlayButtonClicked);
    connect(generateButton, &QPushButton::clicked, this, &MainWindow::onGenerateButtonClicked);
}

MainWindow::~MainWindow()
{
    if (gameInterface)
    {
        delete gameInterface;
        gameInterface = Q_NULLPTR;
    }
    if (gridGenerationInterface)
    {
        delete gridGenerationInterface;
        gridGenerationInterface = Q_NULLPTR;
    }
}

void MainWindow::onPlayButtonClicked()
{
    if (!gameInterface)
    {
        gameInterface = new GameInterface();
        connect(gameInterface, &GameInterface::windowClosed, this, [this](){
            this->show(); });
    }
    gameInterface->show();
    this->hide();
}

void MainWindow::onGenerateButtonClicked()
{
    if (!gridGenerationInterface)
    {
        gridGenerationInterface = new GridGenerationInterface();
        connect(gridGenerationInterface, &GridGenerationInterface::windowClosed, this, [this](){
            this->show(); });
        
    }
    gridGenerationInterface->show();
    this->hide();
}