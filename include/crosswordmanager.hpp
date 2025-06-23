#ifndef CROSSWORDMANAGER_H
#define CROSSWORDMANAGER_H

#include <QVector>
#include <QString>

#include "crosswordcell.hpp"

constexpr double WORD_DENSITY = 0.3;
constexpr char EMPTY_LETTER = '.';
constexpr char CROSSWORD_CELL= '#';

class CrosswordManager
{
public:

    static CrosswordManager &getInstance();
    bool createGrid(int rows, int cols);
    void displayGrid();

    const QVector<QString>& getGrid() const { return grid; }

private:

    explicit CrosswordManager();
    CrosswordManager(const CrosswordManager&) = delete;
    CrosswordManager& operator=(const CrosswordManager&) = delete;


    QVector<QString> grid;
    QVector<CrosswordCell> words;
};

#endif // CROSSWORDMANAGER_H