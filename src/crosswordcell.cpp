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

bool CrosswordCell::isDownWordEnable() const
{
    return wordDown.isEnabled;
}

bool CrosswordCell::isRightWordEnable() const
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

QJsonObject WordToFind::toJson() const
{
    QJsonObject json;
    json["hint"] = hint;
    json["definition"] = definition;
    json["direction"] = (direction == Horizontal ? "horizontal" : "vertical");
    json["solution"] = solution;
    return json;
}

QJsonObject CrosswordCell::toJson() const
{
    QJsonObject cellJson;

    cellJson["x"] = position.x();
    cellJson["y"] = position.y();

    if (wordRight.isEnabled)
        cellJson["wordRight"] = wordRight.toJson();
    if (wordDown.isEnabled)    
        cellJson["wordDown"] = wordDown.toJson();
    return cellJson;
}

void WordToFind::fromJson(const QJsonObject& json, CrosswordCell* crossCell, bool isRightWord)
{
    position.setX(crossCell->x() + (int)(isRightWord == true));
    position.setY(crossCell->y() + (int)(isRightWord == false));
    direction = (json["direction"].toString() == "horizontal" ? Horizontal : Vertical);
    hint = json["hint"].toString();
    definition = json["definition"].toString();
    solution = json["solution"].toString();
    isEnabled = true;
}

void CrosswordCell::fromJson(const QJsonObject& json)
{
    position = QPoint(json["x"].toInt(),json["y"].toInt()) ;

    if (json.contains("wordRight") && json["wordRight"].isObject())
        wordRight.fromJson(json["wordRight"].toObject(),this, true );
    else
        wordRight.isEnabled = false;

    if (json.contains("wordDown") && json["wordDown"].isObject())
        wordDown.fromJson(json["wordDown"].toObject(),this, false);
    else
        wordDown.isEnabled = false;
}

