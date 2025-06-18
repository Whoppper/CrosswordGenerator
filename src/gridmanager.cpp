#include "gridmanager.hpp"
#include "databasemanager.hpp"
#include "logger.hpp"
#include <QDebug>

GridManager* GridManager::instance = nullptr;

GridManager::GridManager()
{

}

GridManager* GridManager::getInstance()
{
    if (instance == nullptr) {
        instance = new GridManager();
    }
    return instance;
}


bool GridManager::createGrid(int rows, int cols)
{
    if (rows <= 0 || cols <= 0)
    {
        Logger::getInstance().log(Logger::LogLevel::Error, "Invalid grid dimensions. Rows and columns must be positive.");
        return false;
    }
    grid.clear();
    grid.resize(rows);

    for (int i = 0; i < rows; ++i)
    {
        grid[i].fill(' ', cols);
    }
    return true;
}
