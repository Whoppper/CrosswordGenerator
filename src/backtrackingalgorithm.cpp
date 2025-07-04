
#include "backtrackingalgorithm.hpp"
#include "crosswordmanager.hpp"
#include "logger.hpp"
#include <QRandomGenerator>


BacktrackingAlgorithm::BacktrackingAlgorithm(int _maxDurationMs) : ISolvingAlgorithmStrategy(_maxDurationMs), visitedState(0)
{

}

bool BacktrackingAlgorithm::solve(CrosswordManager& manager)
{
    visitedState++;
    int currentIndex = manager.getWordsSelectionStrategy()->selectNextWordIndex(manager);
    if (currentIndex == -1)
    {
        Logger::getInstance().log(Logger::LogLevel::Debug, QString("BacktrackingAlgorithm: elapsedTimeMs:%1 . VisitedStates:%2 ")
                                    .arg(elapsedTimeMs()).arg(visitedState));
        return true;
    }
    WordToFind &wordToFind = *manager.words[currentIndex];
    if (elapsedTimeMs() > maxDurationMs || stopSolving)
    {
        Logger::getInstance().log(Logger::LogLevel::Debug, QString("BacktrackingAlgorithm: elapsedTimeMs:%1 . VisitedStates:%2 ")
                                    .arg(elapsedTimeMs()).arg(visitedState));
        QString detail = stopSolving == true ? "Request to stop" : "MaxDurationMs reached";
        throw std::runtime_error(detail.toUtf8().constData());
    }

    QString letters = manager.getWordOnGrid(wordToFind);

    QVector<QString> possibleWords;
    manager.tree->findWordsByPattern(letters, possibleWords);
    if (possibleWords.isEmpty())
        return false;
    std::shuffle(possibleWords.begin(), possibleWords.end(), *QRandomGenerator::global());
    QVector<QVector<Cell>> gridCpy = manager.grid;
    
    for (const QString& word : possibleWords)
    {
        QVector<WordToFind*> wordsToVerify;
        manager.placeWordOnGrid(wordToFind, word, wordsToVerify);
        if (!manager.checkSpecificWordsPossible(wordsToVerify))
        {
            manager.grid = gridCpy;
            continue;
        }
        wordToFind.setPlaced(true);
        bool result = solve(manager);
        if (result)
            return true;
        manager.grid = gridCpy;
        wordToFind.setPlaced(false);
    }
    return false;    
}