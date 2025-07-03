#ifndef CELL_HPP
#define CELL_HPP

#include <QChar>
#include <QVector>

constexpr char EMPTY_LETTER = '.';
constexpr char CROSSWORD_CELL= '#';

class WordToFind;

class Cell
{
public:
    Cell(QChar character = EMPTY_LETTER) : m_character(character) {}

    QChar character() const { return m_character; }
    void setCharacter(QChar character) { m_character = character; }

    void addWordToFind(WordToFind* word)
    {
        if (!m_associatedWords.contains(word))
        {
            m_associatedWords.append(word);
        }
    }

    const QVector<WordToFind*>& associatedWords() const { return m_associatedWords; }

private:
    QChar m_character;
    QVector<WordToFind*> m_associatedWords;
};

#endif // CELL_HPP