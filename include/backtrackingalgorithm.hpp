
#ifndef BACKTRACKINGALGORITHM_HPP
#define BACKTRACKINGALGORITHM_HPP

#include "isolvingalgorithmstrategy.hpp"

class CrosswordManager;

class BacktrackingAlgorithm : public ISolvingAlgorithmStrategy
{
public:
    BacktrackingAlgorithm(int _maxDurationMs);
    bool solve(CrosswordManager& manager) override;

private:
    int visitedState;
    
};

#endif // BACKTRACKINGALGORITHM_HPP

