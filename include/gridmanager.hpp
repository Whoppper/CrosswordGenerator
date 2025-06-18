#ifndef GRIDMANAGER_H
#define GRIDMANAGER_H

#include <QVector>
#include <QString>


class GridManager
{
public:

    static GridManager* getInstance();
    bool createGrid(int rows, int cols);

    const QVector<QString>& getGrid() const { return grid; }

private:

    explicit GridManager();
    GridManager(const GridManager&) = delete;
    GridManager& operator=(const GridManager&) = delete;


    static GridManager* instance;
    QVector<QString> grid;
};

#endif // GRIDMANAGER_H