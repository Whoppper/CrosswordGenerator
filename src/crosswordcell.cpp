#include "crosswordcell.hpp"

void CrosswordCell::enableRightWord(Direction dir)
{
    wordRight.direction = dir;
    wordRight.isEnabled = true;
    wordRight.position.setX(x() + 1);
    wordRight.position.setY(y());
}

void CrosswordCell::enableDownWord(Direction dir)
{
    wordDown.direction = dir;
    wordDown.isEnabled = true;
    wordDown.position.setX(x());
    wordDown.position.setY(y() + 1);
}

bool CrosswordCell::isDownWordEnable()
{
    return wordDown.isEnabled;
}

bool CrosswordCell::isRightWordEnable()
{
    return wordRight.isEnabled;
}

bool WordToFind::isPlaced() const
{
    return isPlacedOk;
}
void WordToFind::setPlaced(bool placed)
{
    isPlacedOk = placed;
}