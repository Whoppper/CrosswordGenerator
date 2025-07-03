#ifndef CROSSWORDMANAGER_H
#define CROSSWORDMANAGER_H

#include "crosswordcell.hpp"
#include "wordtree.hpp"
#include "logger.hpp"

#include <QVector>
#include <QString>
#include <QThread>
#include <chrono>
#include <QJsonObject>
#include <QSharedPointer>

constexpr double WORD_DENSITY = 0.3;
constexpr char EMPTY_LETTER = '.';
constexpr char CROSSWORD_CELL= '#';
constexpr int WORD_MIN_SIZE= 2;
constexpr int GRID_MIN_SIZE= 5;

class DatabaseManager;

class CrosswordManager : public QObject
{
    Q_OBJECT
public:
    explicit CrosswordManager(DatabaseManager* _dbManager, int _maxDurationMs, QSharedPointer<WordTree> sharedWordTree,  QObject *parent = nullptr);
    bool generateGrid(int rows, int cols);
    QString startCrosswordGeneration();
private:

    
    CrosswordManager(const CrosswordManager&) = delete;
    CrosswordManager& operator=(const CrosswordManager&) = delete;

    bool isCrosswordCellPosValid(int x, int y) const;
    QString getWordOnGrid(const WordToFind &word) const;
    void placeWordOnGrid(WordToFind &word, const QString& wordToTry);
    int getNextWordToFindIndex();
    bool areRemainingWordsPossible() const;

    void fillAllWordToFind();
    bool backtracking(int depth);
    void displayGrid(Logger::LogLevel level=Logger::LogLevel::Debug);

    QJsonObject toJson() const;
    QString generateJsonResponse();


    QVector<QString> grid;
    QVector<CrosswordCell> crosswordCells;
    QVector<WordToFind *> words; // pointeurs de crosswordCells, pas besoin de delete
    QSharedPointer<WordTree> tree;
    DatabaseManager* dbManager; // le parent est renseigné pas besoin de delete
    int maxDurationMs;

    std::chrono::time_point<std::chrono::high_resolution_clock> start;

        // a virer
    int maxdepth = 0;
    int visitedGrids;
};

#endif // CROSSWORDMANAGER_H