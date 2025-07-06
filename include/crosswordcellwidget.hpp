#ifndef CROSSWORDCELLWIDGET_HPP
#define CROSSWORDCELLWIDGET_HPP

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>

#include "crosswordcell.hpp"

constexpr int CELL_PIXEL_SIZE = 80;
constexpr int ARROW_MARGING = 8;

class CrosswordCellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CrosswordCellWidget(QWidget *parent = nullptr);

    void setHints(const WordToFind* rightWord, const WordToFind* downWord);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QLabel *rightHintLabel;
    QLabel *downHintLabel;
    QFrame *separatorLine;

    const WordToFind* currentRightWord = nullptr;
    const WordToFind* currentDownWord = nullptr;
};

#endif