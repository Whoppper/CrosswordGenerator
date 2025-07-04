#ifndef CROSSWORDMANAGER_H
#define CROSSWORDMANAGER_H

#include "crosswordcell.hpp"
#include "wordtree.hpp"
#include "cell.hpp"
#include "logger.hpp"
#include "isolvingalgorithmstrategy.hpp"
#include "iwordselectionstrategy.hpp"
#include "logger.hpp"

#include <QVector>
#include <QString>
#include <QThread>
#include <QJsonObject>
#include <QSharedPointer>
#include <QScopedPointer>
#include <chrono>

constexpr double WORD_DENSITY = 0.3;
constexpr int WORD_MIN_SIZE= 2;
constexpr int GRID_MIN_SIZE= 5;

class DatabaseManager;

class CrosswordManager : public QObject
{
    Q_OBJECT

    friend class IWordSelectionStrategy;
    friend class ISolvingAlgorithmStrategy;
    friend class BacktrackingAlgorithm;
    friend class LeastWordCountSelection;
public:
    explicit CrosswordManager(DatabaseManager* _dbManager, int _maxDurationMs, QSharedPointer<WordTree> sharedWordTree,  QObject *parent = nullptr);
    bool generateGrid(int rows, int cols);
    QString startCrosswordGeneration();


    void setWordsSelectionStrategy(IWordSelectionStrategy* strategy);
    void setSolvingAlgorithmStrategy(ISolvingAlgorithmStrategy* strategy);
    IWordSelectionStrategy* getWordsSelectionStrategy() const { return wordSelectionStrategy.data(); }
    ISolvingAlgorithmStrategy* getSolvingAlgorithmStrategy() const { return solvingAlgorithmStrategy.data(); }
private:

    CrosswordManager(const CrosswordManager&) = delete;
    CrosswordManager& operator=(const CrosswordManager&) = delete;

    bool isCrosswordCellPosValid(int x, int y) const;
    QString getWordOnGrid(const WordToFind &word) const;
    void placeWordOnGrid(WordToFind &word, const QString& wordToTry, QVector<WordToFind*>& intersectedWords);
    bool checkSpecificWordsPossible(const QVector<WordToFind*>& wordsToCheck) const;
    void fillAllWordToFind();
    void displayGrid(Logger::LogLevel level=Logger::LogLevel::Debug);

    QJsonObject toJson() const;
    QString generateJsonResponse();

    QVector<QVector<Cell>> grid;
    QVector<CrosswordCell> crosswordCells;
    QVector<WordToFind *> words; // pointeurs de crosswordCells, pas besoin de delete

    QSharedPointer<WordTree> tree;
    DatabaseManager* dbManager; // le parent est renseigné, pas besoin de delete
    QScopedPointer<IWordSelectionStrategy> wordSelectionStrategy;
    QScopedPointer<ISolvingAlgorithmStrategy> solvingAlgorithmStrategy;
};

#endif // CROSSWORDMANAGER_H