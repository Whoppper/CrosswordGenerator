#ifndef IWORDSELECTIONSTRATEGY_HPP
#define IWORDSELECTIONSTRATEGY_HPP

#include <QObject>
#include <QVector>

class CrosswordManager;
class WordToFind;

class IWordSelectionStrategy
{
public:
    virtual ~IWordSelectionStrategy() = default;
    virtual int selectNextWordIndex(const CrosswordManager& manager) const = 0;
};

#endif // IWORDSELECTIONSTRATEGY_HPP