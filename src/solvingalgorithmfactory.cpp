
#include "solvingalgorithmfactory.hpp"
#include "backtrackingalgorithm.hpp"
#include "logger.hpp"
#include <QDebug>

QScopedPointer<ISolvingAlgorithmStrategy> SolvingAlgorithmFactory::createAlgorithm(const QString& name, int maxDurationMs)
{
    if (name == "Standard Backtracking") {
        return QScopedPointer<ISolvingAlgorithmStrategy>(new BacktrackingAlgorithm(maxDurationMs));
    }
    Logger::getInstance().log(Logger::Warning, QString("SolvingAlgorithmFactory: Algorithme inconnu demandé:%1").arg(name));
    return QScopedPointer<ISolvingAlgorithmStrategy>();
}

QStringList SolvingAlgorithmFactory::getAvailableAlgorithms()
{
    QStringList algorithms;
    algorithms << "Standard Backtracking";
    return algorithms;
}