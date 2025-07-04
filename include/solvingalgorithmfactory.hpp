
#ifndef SOLVINGALGORITHMFACTORY_H
#define SOLVINGALGORITHMFACTORY_H

#include <QStringList>
#include <QScopedPointer>
#include "isolvingalgorithmstrategy.hpp"

class SolvingAlgorithmFactory
{
public:
    
    static QScopedPointer<ISolvingAlgorithmStrategy> createAlgorithm(const QString& name, int maxDurationMs);
    static QStringList getAvailableAlgorithms();

private:

    SolvingAlgorithmFactory() = delete;
    Q_DISABLE_COPY(SolvingAlgorithmFactory)
};

#endif // SOLVINGALGORITHMFACTORY_H