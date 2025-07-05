#include "gameinterface.hpp"
#include "crosswordcellwidget.hpp"

GameInterface::GameInterface(QWidget *parent)
    : QWidget(parent),
      gameCrosswordManager(nullptr)
{
    setWindowTitle("Jouer au Mots Fléchés");
    setFixedSize(1000, 800);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QHBoxLayout *topButtonsLayout = new QHBoxLayout();
    loadGridButton = new QPushButton("Charger une grille", this);
    loadGridButton->setFixedHeight(40);
    topButtonsLayout->addWidget(loadGridButton);

    showSolutionButton = new QPushButton("Afficher Solution (Debug)", this);
    showSolutionButton->setFixedHeight(40);
    topButtonsLayout->addWidget(showSolutionButton);
    topButtonsLayout->addStretch();

    mainLayout->addLayout(topButtonsLayout);

    statusLabel = new QLabel("Statut: Aucune grille chargée", this);
    mainLayout->addWidget(statusLabel);

    gridContainer = new QWidget(this);
    gridDisplayLayout = new QGridLayout(gridContainer);
    gridDisplayLayout->setSpacing(0);
    gridDisplayLayout->setContentsMargins(0, 0, 0, 0);
    gridContainer->setLayout(gridDisplayLayout);
    QHBoxLayout *gridWrapper = new QHBoxLayout();
    gridWrapper->addStretch();
    gridWrapper->addWidget(gridContainer);
    gridWrapper->addStretch();
    mainLayout->addLayout(gridWrapper);

    mainLayout->addStretch();

    connect(loadGridButton, &QPushButton::clicked, this, &GameInterface::onLoadGridButtonClicked);
    connect(showSolutionButton, &QPushButton::clicked, this, [this](){
        if (gameCrosswordManager)
            displayGrid(true);
        else
            QMessageBox::information(this, "Aucune grille", "Veuillez d'abord charger une grille.");
    });
    showSolutionButton->setEnabled(true);
}

GameInterface::~GameInterface()
{

    if (gameCrosswordManager)
    {
        delete gameCrosswordManager;
        gameCrosswordManager = nullptr;
    }
}

void GameInterface::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    QWidget::closeEvent(event);
}

void GameInterface::onLoadGridButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Charger une grille de mots croisés", "", "Fichiers JSON de grille (*.json);;Tous les fichiers (*)");
    if (!filePath.isEmpty())
    {
        loadGridFromJson(filePath);
    }
}

void GameInterface::loadGridFromJson(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Erreur de chargement", QString("Impossible d'ouvrir le fichier : %1\n%2").arg(filePath).arg(file.errorString()));
        Logger::getInstance().log(Logger::Error, QString("GameInterface: Impossible d'ouvrir le fichier de grille: %1").arg(file.errorString()));
        statusLabel->setText("Statut: Erreur de chargement du fichier.");
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);

    if (gameCrosswordManager)
    {
        delete gameCrosswordManager;
        gameCrosswordManager = nullptr;
    }

    gameCrosswordManager = new CrosswordManager(nullptr, nullptr, this); 

    if (gameCrosswordManager->fromJson(doc))
    {
        statusLabel->setText(QString("Statut: Grille %1x%2 chargée avec succès.").arg(gameCrosswordManager->grid[0].size()).arg(gameCrosswordManager->grid.size()));
        QMessageBox::information(this, "Grille chargée", "La grille a été chargée avec succès !");
        displayGrid(true);
        Logger::getInstance().log(Logger::Info, QString("GameInterface: Grille chargée: %1x%2").arg(gameCrosswordManager->grid[0].size()).arg(gameCrosswordManager->grid.size()));
    }
    else
    {
        QMessageBox::critical(this, "Erreur de chargement", "Le fichier JSON n'est pas un format de grille valide ou une erreur interne est survenue.");
        Logger::getInstance().log(Logger::Error, "GameInterface: Le fichier JSON n'est pas un format de grille valide.");
        statusLabel->setText("Statut: Erreur de format de grille.");
        if (gameCrosswordManager)
        {
            delete gameCrosswordManager;
            gameCrosswordManager = nullptr;
        }
        return ;
    }
    
}

void GameInterface::displayGrid(bool showSolution)
{
    
    QLayoutItem *item;
    while((item = gridDisplayLayout->takeAt(0)) != nullptr)
    {
        if(item->widget())
        {
            item->widget()->deleteLater();
        }
        delete item;
    }
    cellInputWidgets.clear();

    if (!gameCrosswordManager || gameCrosswordManager->grid.size() == 0 || gameCrosswordManager->grid[0].size() == 0)
        return;

    // debug
    if (showSolution)
    {
        gameCrosswordManager->fillGridWithSolution();
    }
    

    int rows = gameCrosswordManager->grid.size();
    int cols = gameCrosswordManager->grid[0].size();

    int totalWidth = cols * 50;
    int totalHeight = rows * 50;
    gridContainer->setFixedSize(totalWidth, totalHeight);

    QMap<QPoint, CrosswordCell*> crosswordCellsMap;
    for (CrosswordCell& cwc : gameCrosswordManager->crosswordCells) {
        crosswordCellsMap[cwc.pos()] = &cwc;
    }

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            QWidget *cellWidget;
            QChar cellChar = gameCrosswordManager->grid[y][x].character();
            QPoint currentPos(x, y);
            if (crosswordCellsMap.contains(currentPos))
            {
                CrosswordCell* cwc = crosswordCellsMap[currentPos];
                CrosswordCellWidget *crosswordCellWidget = new CrosswordCellWidget();
                
                 WordToFind* rightWord = nullptr;
                if (cwc->isRightWordEnable()) {
                    rightWord = cwc->getRightWordAddr();
                }

                 WordToFind* downWord = nullptr;
                if (cwc->isDownWordEnable()) {
                    downWord = cwc->getDownWordAddr();
                }
                
                crosswordCellWidget->setHints(rightWord, downWord);
                cellWidget = crosswordCellWidget;
            } 
            else
            {
                QLineEdit *inputCell = new QLineEdit();
                inputCell->setFixedSize(50, 50);
                inputCell->setMaxLength(1);
                inputCell->setAlignment(Qt::AlignCenter);
                inputCell->setStyleSheet(
                    "background-color: white;"
                    "border: 1px solid black; "
                    "font-size: 14pt; font-weight: bold; "
                    "padding: 0px; margin: 0px;"
                );
                if (showSolution)
                {
                    inputCell->setText(QString(cellChar).toUpper());
                    inputCell->setReadOnly(true);
                    inputCell->setStyleSheet(inputCell->styleSheet() + " color: blue;");
                }
                else
                {
                    inputCell->setText("");
                    inputCell->setReadOnly(false);
                    connect(inputCell, &QLineEdit::textEdited, this, &GameInterface::onCellEdited);
                }
                
                cellInputWidgets[QPoint(x, y)] = inputCell;
                cellWidget = inputCell;
            }
            cellWidget->setMinimumWidth(0);
            cellWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            gridDisplayLayout->addWidget(cellWidget, y, x);
        }
    }
}

void GameInterface::onCellEdited(const QString &text)
{
    QLineEdit *editedCell = qobject_cast<QLineEdit*>(sender());
    if (editedCell)
    {

        QPoint pos(-1, -1);
        for (auto it = cellInputWidgets.constBegin(); it != cellInputWidgets.constEnd(); ++it) 
        {
            if (it.value() == editedCell)
            {
                pos = it.key();
                break;
            }
        }

        if (pos != QPoint(-1, -1))
        {
            editedCell->setText(text.toUpper());
        }
    }
}
