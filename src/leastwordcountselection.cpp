#include "leastwordcountselection.hpp"
#include "logger.hpp"
#include <limits>

int LeastWordCountSelection::selectNextWordIndex(const CrosswordManager& manager) const
{
    QVector<int> cutoffs = {2,3,5,10, 20, 40, 100, 500, std::numeric_limits<int>::max()};

    for (int currentCutoff : cutoffs)
    {
        for (int i = 0; i < manager.words.size(); ++i)
        {
            if (!manager.words[i]->isPlaced())
            {

                QString currentPattern = manager.getWordOnGrid(*manager.words[i]);
                int numPossibleWords = manager.tree->countWordsByPattern(currentPattern, currentCutoff);

                if (numPossibleWords < currentCutoff)
                {
                    return i;
                }
            }
        }
    }
    return -1;
}