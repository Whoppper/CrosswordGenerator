#ifndef CROSSWORDCELL_H
#define CROSSWORDCELL_H

#include <QString>
#include <QPoint>

enum Direction
    {
        Horizontal,
        Vertical
    };


struct WordToFind
{
    
    WordToFind() = default;

    Direction direction;
    QString wordHint;
    bool isEnabled = false;
};

class CrosswordCell {
public:

    CrosswordCell(int x, int y) : position(x, y) {}

    QPoint pos() const { return position; }

    int x() const { return position.x(); }
    int y() const { return position.y(); }

    void enableRightWord(Direction dir)
    {
        wordRight.direction = dir;
        wordRight.isEnabled = true;
    }
    void enableDownWord(Direction dir)
    {
        wordDown.direction = dir;
        wordDown.isEnabled = true;
    }

private:
    WordToFind wordRight;
    WordToFind wordDown;
    QPoint position;
};

#endif