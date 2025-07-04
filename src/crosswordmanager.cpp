#include "crosswordmanager.hpp"
#include "databasemanager.hpp"
#include "crosswordcell.hpp"
#include "leastwordcountselection.hpp"
#include "backtrackingalgorithm.hpp"

#include <chrono>
#include <random>
#include <limits>

#include <QDebug>
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QJsonArray>


bool CrosswordManager::isCrosswordCellPosValid(int x, int y) const
{
    int rows = grid.size();
    int cols = grid[0].size();
    // pas de cellule sur les bords gauche et haut. pas de cellule dans le coin en bas à droite
    if ((x >= cols - WORD_MIN_SIZE && y >= rows - WORD_MIN_SIZE) || x == 0 || y == 0)
        return false;

    int emptyCellLeft = 0;
    for (int currentX = x-1; currentX >= 0; --currentX)
    {
        if (grid[y][currentX].character() == EMPTY_LETTER)
            emptyCellLeft++;
        else if (grid[y][currentX].character()== CROSSWORD_CELL)
            break;
    }
    if (emptyCellLeft < WORD_MIN_SIZE)
    {
        // peut etre autoriser emptyCellLeft==0
        return false;
    }

    int emptyCellUp = 0;
    for (int currentY = y - 1; currentY >= 0; --currentY)
    {
        if (grid[currentY][x].character() == EMPTY_LETTER)
            emptyCellUp++;
        else if (grid[currentY][x].character() == CROSSWORD_CELL)
            break;
    }
    if (emptyCellUp < WORD_MIN_SIZE)
    {
        // peut etre autoriser emptyCellUp==0
        return false;
    }
    return true;
}


QString CrosswordManager::getWordOnGrid(const WordToFind &word) const
{
    QString letters;
    int x = word.x();
    int y = word.y();
    while (x < grid[0].size() && y < grid.size() && grid[y][x].character() != CROSSWORD_CELL)
    {
        letters.push_back(grid[y][x].character());
        x += word.direction == Direction::Horizontal;
        y += word.direction == Direction::Vertical;
    }
    return letters;
}

void CrosswordManager::placeWordOnGrid(WordToFind &word, const QString& wordToTry , QVector<WordToFind*>& intersectedWords)
{
    int x = word.x();
    int y = word.y();
    for (QChar letter : wordToTry) 
    {
        if (grid[y][x].character() == EMPTY_LETTER)
        {
            for (WordToFind* associatedWord : grid[y][x].associatedWords())
            {
                if (associatedWord != &word)
                {
                    intersectedWords.push_back(associatedWord);
                }
            }
        }
        grid[y][x].setCharacter(letter);
        x += word.direction == Direction::Horizontal;
        y += word.direction == Direction::Vertical;
    }
}


CrosswordManager::CrosswordManager(DatabaseManager* _dbManager, QSharedPointer<WordTree> sharedWordTree, QObject *parent)
    : QObject(parent),
      tree(sharedWordTree),
      dbManager(_dbManager)
{
    if (!dbManager)
    {
        Logger::getInstance().log(Logger::Critical, "CrosswordManager créé sans DatabaseManager valide !");
    }
    Logger::getInstance().log(Logger::Debug, QString("CrosswordManager créé dans thread ID: %1")
                                .arg((qintptr)QThread::currentThreadId()));

}

void CrosswordManager::setWordsSelectionStrategy(const QString &heuristicName)
{
    wordSelectionStrategy.reset(WordSelectionFactory::createWordSelection(heuristicName).take());
}

void CrosswordManager::setSolvingAlgorithmStrategy(const QString &algorithmName, int duration)
{
    solvingAlgorithmStrategy.reset(SolvingAlgorithmFactory::createAlgorithm(algorithmName, duration).take());
}



bool CrosswordManager::generateGrid(int rows, int cols)
{
    if ((rows < GRID_MIN_SIZE || cols < GRID_MIN_SIZE ))
    {
        Logger::getInstance().log(Logger::LogLevel::Error, "Invalid grid dimensions.");
        return false;
    }
    grid.clear();
    grid.resize(rows);
    crosswordCells.clear();

    for (int i = 0; i < rows; ++i)
    {
        grid[i].resize(cols);
        for(int j = 0; j < cols; ++j) {
            grid[i][j].setCharacter(EMPTY_LETTER);
        }
    }

    QRandomGenerator *rng = QRandomGenerator::global();

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            if (x == 0 && y == 0)
            {
                grid[y][x].setCharacter(CROSSWORD_CELL);
                crosswordCells.append(CrosswordCell(x, y));
                crosswordCells.back().enableRightWord(Direction::Vertical);
                crosswordCells.back().enableDownWord(Direction::Horizontal);
            }
            else if (y == 0 && x % 2 == 0)
            {
                grid[y][x].setCharacter(CROSSWORD_CELL);
                crosswordCells.append(CrosswordCell(x, y));
                if (x != cols - 1)
                    crosswordCells.back().enableRightWord(Direction::Vertical);
                crosswordCells.back().enableDownWord(Direction::Vertical);
            }
            else if (x == 0 && y % 2 == 0)
            {
                grid[y][x].setCharacter(CROSSWORD_CELL);
                crosswordCells.append(CrosswordCell(x, y));
                crosswordCells.back().enableRightWord(Direction::Horizontal);
                if (y != rows - 1)
                    crosswordCells.back().enableDownWord(Direction::Horizontal);
            }
            else if (rng->generateDouble() < WORD_DENSITY
                    && isCrosswordCellPosValid(x, y))  
            {
                grid[y][x].setCharacter(CROSSWORD_CELL);
                crosswordCells.append(CrosswordCell(x, y));
                if (x < cols - WORD_MIN_SIZE)
                    crosswordCells.back().enableRightWord(Direction::Horizontal);
                if (y < rows - WORD_MIN_SIZE)
                    crosswordCells.back().enableDownWord(Direction::Vertical);
            }
        }
    }
    return true;
}


void CrosswordManager::fillAllWordToFind()
{
    words.clear();
    for (int y = 0; y < grid.size(); ++y)
    {
        for (int x = 0; x < grid[y].size(); ++x)
        {
            grid[y][x] = Cell(grid[y][x].character());
        }
    }

    for (CrosswordCell &cwc : crosswordCells)
    {
        if (cwc.isRightWordEnable())
        {
            words.push_back(cwc.getRightWordAddr());
            int currentX = cwc.x();
            int currentY = cwc.y();
            while (currentX < grid[0].size() && currentY < grid.size() && grid[currentY][currentX].character() != CROSSWORD_CELL)
            {
                grid[currentY][currentX].addWordToFind(cwc.getRightWordAddr());
                currentY += cwc.getRightWordAddr()->direction == Direction::Vertical;
                currentX += cwc.getRightWordAddr()->direction == Direction::Horizontal;
            }
        }
        if (cwc.isDownWordEnable())
        {
            words.push_back(cwc.getDownWordAddr());
            int currentX = cwc.x();
            int currentY = cwc.y();
            while (currentX < grid[0].size() && currentY < grid.size() && grid[currentY][currentX].character() != CROSSWORD_CELL)
            {
                grid[currentY][currentX].addWordToFind(cwc.getDownWordAddr());
                currentY += cwc.getDownWordAddr()->direction == Direction::Vertical;
                currentX += cwc.getDownWordAddr()->direction == Direction::Horizontal;
            }
        }
    }
}

QString CrosswordManager::startCrosswordGeneration()
{
    if (!solvingAlgorithmStrategy || !wordSelectionStrategy)
    {
        Logger::getInstance().log(Logger::LogLevel::Debug, QString("Crossword generation interrupted: algoruthm or heuristic are missing"));
        return "";
    }
        
    bool isOk = false;
    Logger::getInstance().log(Logger::LogLevel::Debug, "startCrosswordGeneration()");
    fillAllWordToFind();
    try
    {
        solvingAlgorithmStrategy->startTimer();
        isOk = solvingAlgorithmStrategy->solve(*this);
    }
    catch (const std::exception& e)
    {
        Logger::getInstance().log(Logger::LogLevel::Debug, QString("Crossword generation interrupted: %1").arg(e.what()));
    }
    Logger::getInstance().log(Logger::LogLevel::Debug, QString("Algorithm finished success: %0.").arg((int)isOk));
    displayGrid(Logger::LogLevel::Debug);

    if (isOk)
    {
        QString response = generateJsonResponse();
        return response;
    }
    return QString();
}

QString CrosswordManager::generateJsonResponse()
{
    for (int i = 0; i < words.size(); ++i)
    {
        QString currentPattern = getWordOnGrid(*words[i]);
        QPair<QString, QString> details = dbManager->getWordDetails(currentPattern);
        words[i]->solution = currentPattern;
        words[i]->definition = details.first;
        words[i]->hint = details.second;
    }

    QJsonObject crosswordData = toJson();
    QJsonDocument doc(crosswordData);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact); // Indented Compact
    QString jsonString = QString::fromUtf8(jsonData);
    return jsonString;
}

void CrosswordManager::displayGrid(Logger::LogLevel level)
{
    for (auto &row : grid)
    {
        QString rowString;
        for (const Cell& cell : row)
        {
            rowString.append(cell.character());
        }
        qDebug() << qPrintable(rowString);
        Logger::getInstance().log(level, rowString);
    }
    qDebug() << "";
    Logger::getInstance().log(level, "\n");
    
}


bool CrosswordManager::checkSpecificWordsPossible(const QVector<WordToFind*>& wordsToCheck) const
{
    for (const WordToFind* wordPtr : wordsToCheck)
    {
        QString currentPattern = getWordOnGrid(*wordPtr);
        bool isPossible = tree->findAnyWordByPattern(currentPattern);
        if (!isPossible)
        {
            return false;
        }
    }
    return true;
}


QJsonObject CrosswordManager::toJson() const
{
    QJsonObject crosswordJson;

    if (!grid.isEmpty())
    {
        crosswordJson["rows"] = grid.size();
        crosswordJson["cols"] = grid[0].size();
    }

    QJsonArray cellsArray;
    for (const CrosswordCell& cell : crosswordCells)
    {
        cellsArray.append(cell.toJson());
    }
    crosswordJson["cells"] = cellsArray;
    return crosswordJson;
}