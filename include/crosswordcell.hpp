#ifndef CROSSWORDCELL_H
#define CROSSWORDCELL_H

#include <QString>
#include <QPoint>

enum Direction
{
    Horizontal,
    Vertical
};


class WordToFind
{
    public:

    int x() const { return position.x(); }
    int y() const { return position.y(); }
    bool isPlaced() const;
    void setPlaced(bool placed);
    
    WordToFind() = default;
    Direction direction;
    QString hint;
    bool isEnabled = false;
    QPoint position;
    bool isPlacedOk = false;
    
    
};

class CrosswordCell {
public:

    CrosswordCell(int x, int y) : position(x, y) {}

    QPoint pos() const { return position; }
    int x() const { return position.x(); }
    int y() const { return position.y(); }
    WordToFind getRightWord() {return wordRight;}
    WordToFind getDownWord() {return wordDown;}

    void enableRightWord(Direction dir);
    void enableDownWord(Direction dir);
    bool isDownWordEnable();
    bool isRightWordEnable();


private:
    WordToFind wordRight;
    WordToFind wordDown;
    QPoint position;
};

#endif