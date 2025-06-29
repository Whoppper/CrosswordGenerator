#include "crosswordmanager.hpp"
#include "databasemanager.hpp"
#include "crosswordcell.hpp"

#include <random> 
#include <chrono>
#include <limits>
#include <QDebug>
#include <QRandomGenerator>


bool CrosswordManager::isCrosswordCellPosValid(int x, int y)
{
    int nbEmptyCellLeft = 0;
    int nbEmptyCellUp = 0;
    int tmpX = x;
    int tmpY = y;

    if (grid.size() == 0)
        return false;

    int rows = grid.size();
    int cols = grid[0].size();
    if ((x >= cols - 2 && y >= rows - 2) || x == 0 || y == 0)
        return false;

    while (--x >= 0)
    {
        if (grid[y][x] == EMPTY_LETTER)
        nbEmptyCellLeft++;
        else if (grid[y][x] == CROSSWORD_CELL)
            break;
    }
    if (nbEmptyCellLeft <= 1)
        return false;
    x = tmpX;

    while (--y >= 0)
    {
        if (grid[y][x] == EMPTY_LETTER)
        nbEmptyCellUp++;
        else if (grid[y][x] == CROSSWORD_CELL)
            break;
    }
    if (nbEmptyCellUp <= 1)
        return false;
    y = tmpY;
    return true;
}


QString CrosswordManager::getWordOnGrid(const WordToFind &word)
{
    QString letters;
    int x = word.x();
    int y = word.y();
    while (x < grid[0].size() && y < grid.size() && grid[y][x] != CROSSWORD_CELL)
    {
        letters.push_back(grid[y][x]);
        x += word.direction == Direction::Horizontal;
        y += word.direction == Direction::Vertical;
    }
    return letters;
}

void CrosswordManager::placeWordOnGrid(WordToFind &word, const QString& wordToTry)
{
    int x = word.x();
    int y = word.y();
    for (auto &letter : wordToTry) 
    {
        grid[y][x] = letter;
        x += word.direction == Direction::Horizontal;
        y += word.direction == Direction::Vertical;
    }
}



CrosswordManager::CrosswordManager(DatabaseManager* _dbManager, int _maxDurationMs, QObject *parent)
    : QObject(parent),
      dbManager(_dbManager),
      maxDurationMs(_maxDurationMs)
{
    if (!dbManager)
    {
        Logger::getInstance().log(Logger::Critical, "CrosswordManager créé sans DatabaseManager valide !");
    }
    Logger::getInstance().log(Logger::Debug, QString("CrosswordManager créé dans thread ID: %1 avec durée max: %2ms.")
                                .arg((qintptr)QThread::currentThreadId())
                                .arg(maxDurationMs));
}



bool CrosswordManager::generateGrid(int rows, int cols)
{
    if ((rows <= 4 || cols <= 4 ) && rows % 2 == 0 && cols % 2 == 0)
    {
        Logger::getInstance().log(Logger::LogLevel::Error, "Invalid grid dimensions. Rows and columns must be >4 and even.");
        return false;
    }
    grid.clear();
    grid.resize(rows);

    for (int i = 0; i < rows; ++i)
    {
        grid[i].fill(EMPTY_LETTER, cols);
    }

    std::uniform_real_distribution<> dist(0.0, 1.0);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            if (x == 0 && y == 0)
            {
                grid[y][x]= CROSSWORD_CELL;
                crosswordCells.append(CrosswordCell(x, y));
                crosswordCells.back().enableRightWord(Direction::Vertical);
                crosswordCells.back().enableDownWord(Direction::Horizontal);
            }
            else if (y == 0 && x % 2 == 0)
            {
                grid[y][x]= CROSSWORD_CELL;
                crosswordCells.append(CrosswordCell(x, y));
                crosswordCells.back().enableRightWord(Direction::Vertical);
                crosswordCells.back().enableDownWord(Direction::Vertical);
            }
            else if (x == 0 && y % 2 == 0)
            {
                grid[y][x]= CROSSWORD_CELL;
                crosswordCells.append(CrosswordCell(x, y));
                crosswordCells.back().enableRightWord(Direction::Horizontal);
                crosswordCells.back().enableDownWord(Direction::Horizontal);
            }
            else if (dist(rng) < WORD_DENSITY
                    && isCrosswordCellPosValid(x, y))  
            {
                grid[y][x]= CROSSWORD_CELL;
                crosswordCells.append(CrosswordCell(x, y));
                if (x < cols - 2)
                    crosswordCells.back().enableRightWord(Direction::Horizontal);
                if (y < rows - 2)
                    crosswordCells.back().enableDownWord(Direction::Vertical);
            }
        }
    }
    return true;
}





bool CrosswordManager::backtracking(int depth)
{
    visitedGrids++;
    if (depth > maxdepth)
    {
        maxdepth = depth;
        //displayGrid();
    }
        
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    int currentIndex = getNextWordToFindIndex();
    if (currentIndex == -1)
    {
        return true;
    }

    WordToFind &wordToFind = words[currentIndex];
    if (duration_ms > maxDurationMs)
    {
        Logger::getInstance().log(Logger::LogLevel::Debug, QString("current word. y:%0  x:%1 ").arg(wordToFind.y()).arg(wordToFind.x()));
        Logger::getInstance().log(Logger::LogLevel::Debug, QString("max depth:%0").arg(maxdepth));
        throw std::runtime_error("maxDurationMs reached");
    }
    
    QString letters = getWordOnGrid(wordToFind);

    QVector<QString> possibleWords;
    tree.findWordsByPattern(letters, possibleWords);
    if (possibleWords.isEmpty())
        return false;

    std::shuffle(possibleWords.begin(), possibleWords.end(), *QRandomGenerator::global());
    QVector<QString> gridCpy = grid;
    for (const QString& word : possibleWords)
    {
        placeWordOnGrid(wordToFind, word);
        if (!areRemainingWordsPossible())
        {
            grid = gridCpy;
            continue;
        }

        wordToFind.setPlaced(true);
        bool result = backtracking(depth + 1);
        if (result == true)
            return true;
        grid = gridCpy;
        wordToFind.setPlaced(false);
    }
    //Logger::getInstance().log(Logger::LogLevel::Debug, QString("current depth: %0 ").arg(depth));
    return false;    
}

void CrosswordManager::fillAllWordToFind()
{
    words.clear();
    for (CrosswordCell &cwc : crosswordCells)
    {
        if (cwc.isRightWordEnable())
        {
            words.push_back(cwc.getRightWord());
        }
        if (cwc.isDownWordEnable())
        {
            words.push_back(cwc.getDownWord());
        }
    }

}

QString CrosswordManager::startCrosswordGeneration()
{
    bool isOk = false;
    Logger::getInstance().log(Logger::LogLevel::Debug, "startCrosswordGeneration()");
    start = std::chrono::high_resolution_clock::now();
    fillAllWordToFind();
    createWordsTree();
    visitedGrids = 0;
    try
    {
        isOk = backtracking(0);
    }
    catch (const std::exception& e)
    {
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    Logger::getInstance().log(Logger::LogLevel::Info, QString("backtracting finished. success: %0.").arg((int)isOk));
    Logger::getInstance().log(Logger::LogLevel::Info, QString("Exécution time : %0 ms").arg(duration_ms));
    Logger::getInstance().log(Logger::LogLevel::Info, QString("Visited grids : %0 ").arg(visitedGrids));
    displayGrid(Logger::LogLevel::Info);
    return QString();
}

void CrosswordManager::displayGrid(Logger::LogLevel level)
{
    for (auto &row : grid)
    {
        //qDebug() << qPrintable(row);
        Logger::getInstance().log(level, row);
    }
    //qDebug() << "";
    Logger::getInstance().log(level, "\n");
    
}


void CrosswordManager::createWordsTree()
{
    QVector<QString> allWords;
    dbManager->fillWordsList(allWords);
    for (const QString &word : allWords)
    {
        tree.insert(word);
    }
}


int CrosswordManager::getNextWordToFindIndex()
{
    int bestIndex = -1;
    QVector<int> cutoffs = {2,3,5,10, 20, 40, 100, 500, std::numeric_limits<int>::max()};

    for (int currentCutoff : cutoffs)
    {
        for (int i = 0; i < words.size(); ++i)
        {
            if (!words[i].isPlaced())
            {
                QString currentPattern = getWordOnGrid(words[i]);
                int numPossibleWords = tree.countWordsByPattern(currentPattern, currentCutoff);

                if (numPossibleWords < currentCutoff)
                {
                    return i;
                }
            }
        }
    }
    return bestIndex;
}

bool CrosswordManager::areRemainingWordsPossible()
{
    for (int i = 0; i < words.size(); ++i)
    {
        if (!words[i].isPlaced())
        {
            QString currentPattern = getWordOnGrid(words[i]);
            bool isOk = tree.findAnyWordByPattern(currentPattern);
            if (isOk == false)
            {
                return false;
            }
        }
    }
    return true;
}