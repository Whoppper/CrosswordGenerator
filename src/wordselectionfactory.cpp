#include "wordselectionfactory.hpp"
#include "leastwordcountselection.hpp" 

QScopedPointer<IWordSelectionStrategy> WordSelectionFactory::createWordSelection(const QString& name)
{
    if (name == "Least Word Count")
    { 
        return QScopedPointer<IWordSelectionStrategy>( new LeastWordCountSelection());
    }
    Logger::getInstance().log(Logger::Warning, QString("WordSelectionFactory: WordSelection inconnu demandé:%1").arg(name));
    return QScopedPointer<IWordSelectionStrategy>(); 
}

QStringList WordSelectionFactory::getAvailableWordSelection()
{
    QStringList wordSelection;
    wordSelection << "Least Word Count";
    return wordSelection;
}