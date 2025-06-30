#ifndef CROSSWORDCELL_H
#define CROSSWORDCELL_H

#include <QString>
#include <QPoint>
#include <QJsonObject>

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
    QJsonObject toJson() const;
    
    WordToFind() = default;
    Direction direction;
    bool isEnabled = false;
    QPoint position;
    bool isPlacedOk = false;
    
    QString hint;
    QString definition;
    QString solution;
};

class CrosswordCell {
public:

    CrosswordCell(int x, int y) : position(x, y) {}

    QPoint pos() const { return position; }
    int x() const { return position.x(); }
    int y() const { return position.y(); }
    WordToFind *getRightWordAddr() {return &wordRight;}
    WordToFind *getDownWordAddr() {return &wordDown;}

    void enableRightWord(Direction dir);
    void enableDownWord(Direction dir);
    bool isDownWordEnable();
    bool isRightWordEnable();
    QJsonObject toJson() const;


private:
    WordToFind wordRight;
    WordToFind wordDown;
    QPoint position;
};

#endif