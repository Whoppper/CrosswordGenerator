#ifndef CROSSWORDMANAGER_H
#define CROSSWORDMANAGER_H

#include <QVector>
#include <QString>
#include <chrono>

#include "crosswordcell.hpp"

constexpr double WORD_DENSITY = 0.2;
constexpr char EMPTY_LETTER = '.';
constexpr char CROSSWORD_CELL= '#';

constexpr int MAX_TIME_ALLOWED= 1000000;

class CrosswordManager
{
public:

    static CrosswordManager &getInstance();
    bool createGrid(int rows, int cols);
    void displayGrid();

    const QVector<QString>& getGrid() const { return grid; }
    bool setUpGrid();
    bool backtracking(int index);
private:

    explicit CrosswordManager();
    CrosswordManager(const CrosswordManager&) = delete;
    CrosswordManager& operator=(const CrosswordManager&) = delete;

    void generateWords();


    QVector<QString> grid;
    QVector<CrosswordCell> crosswordCells;

    QVector<WordToFind> words;

    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

#endif // CROSSWORDMANAGER_H