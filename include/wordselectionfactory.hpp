#ifndef WORDSELECTIONFACTORY_HPP
#define WORDSELECTIONFACTORY_HPP

#include <QStringList>
#include <QScopedPointer> 
#include "iwordselectionstrategy.hpp"

class WordSelectionFactory
{
public:

    static QScopedPointer<IWordSelectionStrategy> createWordSelection(const QString& name);
    static QStringList getAvailableWordSelection();

private:

    WordSelectionFactory() = delete;
    Q_DISABLE_COPY(WordSelectionFactory)
};

#endif // WORDSELECTIONFACTORY_HPP