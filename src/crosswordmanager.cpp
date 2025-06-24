#include "crosswordmanager.hpp"
#include "databasemanager.hpp"
#include "crosswordcell.hpp"
#include "logger.hpp"

#include <random> 
#include <chrono> 
#include <QDebug>
#include <QRandomGenerator>


namespace
{

    bool isPosValid(int x, int y,  QVector<QString> &grid)
    {
        int nbEmptyCellLeft = 0;
        int nbEmptyCellUp = 0;
        int tmpX = x;
        int tmpY = y;

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


    QString getWordOnGrid(WordToFind &word, QVector<QString> &grid)
    {
        QString letters;
        int x = word.x();
        int y = word.y();
        while (x < grid[0].size() && y < grid.size() && grid[y][x] != CROSSWORD_CELL)
        {
            letters.push_back(grid[y][x]);
            x += word.direction == Direction::Horizontal;
            y += word.direction == Direction::Vertical;
            //qDebug() << "coord " << x << " " << y;
        }
        //qDebug() << "letters" << letters;
        return letters;
    }

    void placeWordOnGrid(WordToFind &word, const QString& wordToTry, QVector<QString> &grid)
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

}

CrosswordManager::CrosswordManager()
{

}

CrosswordManager& CrosswordManager::getInstance()
{
    static CrosswordManager instance;
    return instance;
}



bool CrosswordManager::createGrid(int rows, int cols)
{
    if (rows <= 4 || cols <= 4)
    {
        Logger::getInstance().log(Logger::LogLevel::Error, "Invalid grid dimensions. Rows and columns must be >4.");
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
            // at col:0 every two rows, we have a crosswordCell
            // at row:0 every two cols, we have a crosswordCell
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
                    && isPosValid(x, y, grid) 
                    && (x < cols - 2 || y < rows - 2)
                    && (x != 0 && y != 0)) 
            {
                grid[y][x]= CROSSWORD_CELL;
                crosswordCells.append(CrosswordCell(x, y));
                if (x != cols - 2)
                    crosswordCells.back().enableRightWord(Direction::Horizontal);
                if (y != rows - 2)
                    crosswordCells.back().enableDownWord(Direction::Vertical);
            }
        }
    }
    return true;
}

bool CrosswordManager::backtracking(int index)
{
    displayGrid();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (duration_ms > MAX_TIME_ALLOWED)
    {
        throw std::runtime_error("time limite reached");
    }
    if (index == words.size())
    {
        displayGrid();
        throw std::runtime_error("youpi");
        return true;
    }
    WordToFind &wordToFind = words[index];
    QString letters = getWordOnGrid(wordToFind, grid);
    //qDebug() << "letters on the grid: " << letters;
    QList<QString> possibleWords = DatabaseManager::getInstance().searchWordByPattern(letters);
    if (possibleWords.isEmpty())
    {
        //Logger::getInstance().log(Logger::LogLevel::Debug, "Aucun mot possible trouvé, mauvais chemin de backtracking");
        return false;
    }
    std::shuffle(possibleWords.begin(), possibleWords.end(), *QRandomGenerator::global());
    QVector<QString> gridCpy = grid;
    for (const QString& word : possibleWords)
    {
        qDebug() << "on place le mot: " << word;
        placeWordOnGrid(wordToFind, word , grid);
        backtracking(index + 1);
        grid = gridCpy;
    }
    return false;    
}

void CrosswordManager::generateWords()
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

bool CrosswordManager::setUpGrid()
{
    Logger::getInstance().log(Logger::LogLevel::Debug, "setUpGrid()");
    start = std::chrono::high_resolution_clock::now();
    generateWords();
    try
    {
        backtracking(0);

    
    }
    catch (const std::exception& e)
    {

        auto end = std::chrono::high_resolution_clock::now();
        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        Logger::getInstance().log(Logger::LogLevel::Error, "Exception catch. crossword generation failed.");
        Logger::getInstance().log(Logger::LogLevel::Error, QString("Exécution time : {1} ms").arg(duration_ms));
        return false;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    Logger::getInstance().log(Logger::LogLevel::Debug, "backtracting finished. Crossword generation ok.");
    Logger::getInstance().log(Logger::LogLevel::Debug, QString("Exécution time : {1} ms").arg(duration_ms));
    return true;
}

void CrosswordManager::displayGrid()
{
    for (auto &row : grid)
    {
        qDebug() << row;
    }
    qDebug() << "\n\n";
}

