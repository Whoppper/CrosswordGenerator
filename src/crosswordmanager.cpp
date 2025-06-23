#include "crosswordmanager.hpp"
#include "databasemanager.hpp"
#include "crosswordcell.hpp"
#include "logger.hpp"

#include <random> 
#include <chrono> 
#include <QDebug>


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
        Logger::getInstance().log(Logger::LogLevel::Error, "Invalid grid dimensions. Rows and columns must be positive.");
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
                words.append(CrosswordCell(x, y));
                words.back().enableRightWord(Direction::Vertical);
                words.back().enableDownWord(Direction::Horizontal);
            }
            else if (y == 0 && x % 2 == 0)
            {
                grid[y][x]= CROSSWORD_CELL;
                words.append(CrosswordCell(x, y));
                words.back().enableRightWord(Direction::Vertical);
                words.back().enableDownWord(Direction::Vertical);
            }
            else if (x == 0 && y % 2 == 0)
            {
                grid[y][x]= CROSSWORD_CELL;
                words.append(CrosswordCell(x, y));
                words.back().enableRightWord(Direction::Horizontal);
                words.back().enableDownWord(Direction::Horizontal);
            }
            else if (dist(rng) < WORD_DENSITY
                    && isPosValid(x, y, grid) 
                    && (x < cols - 2 || y < rows - 2)
                    && (x != 0 && y != 0)) 
            {
                grid[y][x]= CROSSWORD_CELL;
                words.append(CrosswordCell(x, y));
                if (x != cols - 2)
                    words.back().enableRightWord(Direction::Horizontal);
                if (y != rows - 2)
                    words.back().enableDownWord(Direction::Vertical);
            }
        }
    }
    return true;

}

void CrosswordManager::displayGrid()
{
    for (auto &row : grid)
    {
        qDebug() << row;
    }
}

