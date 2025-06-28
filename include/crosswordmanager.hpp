#ifndef CROSSWORDMANAGER_H
#define CROSSWORDMANAGER_H

#include "crosswordcell.hpp"
#include "wordtree.hpp"

#include <QVector>
#include <QString>
#include <chrono>

constexpr double WORD_DENSITY = 0.3;
constexpr char EMPTY_LETTER = '.';
constexpr char CROSSWORD_CELL= '#';
constexpr int MAX_TIME_ALLOWED= 100000; //ms

class CrosswordManager
{
public:

    static CrosswordManager &getInstance();
    bool createGrid(int rows, int cols);
    void displayGrid();
    const QVector<QString>& getGrid() const { return grid; }
    bool startCrosswordGeneration();
    
private:

    explicit CrosswordManager();
    CrosswordManager(const CrosswordManager&) = delete;
    CrosswordManager& operator=(const CrosswordManager&) = delete;

    bool isCrosswordCellPosValid(int x, int y);
    QString getWordOnGrid(const WordToFind &word);
    void placeWordOnGrid(WordToFind &word, const QString& wordToTry);
    int getNextWordToFindIndex();
    bool areRemainingWordsPossible();

    void fillAllWordToFind();
    void createWordsTree();
    bool backtracking(int depth);


    QVector<QString> grid;
    QVector<CrosswordCell> crosswordCells;
    QVector<WordToFind> words;
    WordTree tree;
    

    std::chrono::time_point<std::chrono::high_resolution_clock> start;

        // a virer
    int maxdepth = 0;
    int visitedGrids;
};

#endif // CROSSWORDMANAGER_H