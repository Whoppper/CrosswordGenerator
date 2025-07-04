#ifndef LEASTWORDSELECTION_HPP
#define LEASTWORDSELECTION_HPP

#include "iwordselectionstrategy.hpp"
#include "crosswordmanager.hpp"
#include "wordtree.hpp"

class LeastWordCountSelection : public IWordSelectionStrategy
{
public:
    int selectNextWordIndex(const CrosswordManager& manager) const override;
};

#endif // LEASTWORDSELECTION_HPP

