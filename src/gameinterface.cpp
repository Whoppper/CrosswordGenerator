#include "gameinterface.hpp"

GameInterface::GameInterface(QWidget *parent)
    : QWidget(parent),
      gameCrosswordManager(nullptr)
{
    setWindowTitle("Jouer au Mots Croisés");
    setFixedSize(1000, 800);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    loadGridButton = new QPushButton("Charger une grille", this);
    loadGridButton->setFixedHeight(40);
    mainLayout->addWidget(loadGridButton);

    statusLabel = new QLabel("Statut: Aucune grille chargée", this);
    mainLayout->addWidget(statusLabel);

    gridDisplayLayout = new QGridLayout();
    mainLayout->addLayout(gridDisplayLayout);
    mainLayout->addStretch();

    connect(loadGridButton, &QPushButton::clicked, this, &GameInterface::onLoadGridButtonClicked);
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
        
        displayGrid();
        updateHintsDisplay();
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
    }
}

void GameInterface::displayGrid()
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


    int rows = gameCrosswordManager->grid.size();
    int cols = gameCrosswordManager->grid[0].size();

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            QWidget *cellWidget;
            QChar cellChar = gameCrosswordManager->grid[y][x].character();

            if (cellChar == CROSSWORD_CELL)
            {
                QLabel *blackCell = new QLabel();
                blackCell->setFixedSize(30, 30);
                blackCell->setStyleSheet("background-color: black; border: 1px solid gray;");
                cellWidget = blackCell;
            } 
            else
            {
                QLineEdit *inputCell = new QLineEdit();
                inputCell->setFixedSize(30, 30);
                inputCell->setMaxLength(1);
                inputCell->setAlignment(Qt::AlignCenter);
                inputCell->setStyleSheet("background-color: white; border: 1px solid gray; font-weight: bold;");
                inputCell->setText("");

                connect(inputCell, &QLineEdit::textEdited, this, &GameInterface::onCellEdited);
                
                cellInputWidgets[QPoint(x, y)] = inputCell;
                cellWidget = inputCell;
            }
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

void GameInterface::updateHintsDisplay()
{
    // C'est un point de départ. Vous voudrez probablement un QListWidget
    // ou une zone de texte dédiée pour afficher les indices.
    // Pour l'instant, on peut juste les logger ou les afficher dans un QLabel simple.

    // Vous pouvez créer un nouveau layout pour les indices si vous voulez
    // qu'ils soient affichés à côté ou en dessous de la grille.

    // Par exemple, ajoutons un QLabel pour l'instant (à améliorer)
    /*QLabel *hintsLabel = new QLabel("Indices des mots:\n", this);
    QString hintsText = "Indices des mots:\n";
    
    if (gameCrosswordManager) {
        for (const WordToFind* word : gameCrosswordManager->getAllWords()) {
            hintsText += QString("- **(%1,%2) %3**: %4 (Solution: %5)\n")
                         .arg(word->x())
                         .arg(word->y())
                         .arg(word->direction == Horizontal ? "Horizontal" : "Vertical")
                         .arg(word->hint.isEmpty() ? word->definition : word->hint) // Utiliser l'indice s'il existe, sinon la définition
                         .arg(word->solution); // Pour le debug, on affiche la solution

        }
    }
    hintsLabel->setText(hintsText);
    hintsLabel->setWordWrap(true); */ // Permet le retour à la ligne automatique
    // Ajoutez ce label à votre mainLayout, peut-être dans un QScrollArea si les indices sont nombreux.
    // mainLayout->addWidget(hintsLabel); // Décommenter si vous voulez juste les voir pour le moment.
    // Ou mieux, placez-le dans un layout dédié à côté de la grille.

    // Vous devrez probablement gérer l'ajout/suppression de ce widget si des grilles sont rechargées.
}