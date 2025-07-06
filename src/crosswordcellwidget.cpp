#include "crosswordcellwidget.hpp"
#include <QPainter>
#include <QDebug>

CrosswordCellWidget::CrosswordCellWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(CELL_PIXEL_SIZE, CELL_PIXEL_SIZE);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, ARROW_MARGING, ARROW_MARGING);
    mainLayout->setSpacing(0);

    setStyleSheet(
        "background-color: lightgray; "
        "border: 1px solid black; "
        "font-size: 14pt; font-weight: bold;"
        "padding: 0px; margin: 0px;"
    );

    rightHintLabel = new QLabel(this);
    rightHintLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    rightHintLabel->setStyleSheet("font-size: 7pt; font-weight: bold; color: darkblue;");
    rightHintLabel->setWordWrap(true);
    mainLayout->addWidget(rightHintLabel);

    separatorLine = new QFrame(this);
    separatorLine->setFrameShape(QFrame::HLine);
    separatorLine->setFrameShadow(QFrame::Sunken);
    separatorLine->setLineWidth(1);
    separatorLine->setVisible(false);

    mainLayout->addWidget(separatorLine);

    downHintLabel = new QLabel(this);
    downHintLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    downHintLabel->setStyleSheet("font-size: 7pt; font-weight: bold; color: darkgreen;");
    downHintLabel->setWordWrap(true);
    mainLayout->addWidget(downHintLabel);
}

void CrosswordCellWidget::setHints(const WordToFind* rightWord, const WordToFind* downWord)
{
    currentRightWord = rightWord;
    currentDownWord = downWord;

    rightHintLabel->setText("");
    downHintLabel->setText("");
    separatorLine->setVisible(false);

    if (rightWord) {
        rightHintLabel->setText(rightWord->hint.isEmpty() ? rightWord->solution : rightWord->hint);
        rightHintLabel->show();
    } else {
        rightHintLabel->hide();
    }

    if (downWord) {
        downHintLabel->setText(downWord->hint.isEmpty() ? downWord->solution : downWord->hint);
        downHintLabel->show();
    } else {
        downHintLabel->hide();
    }

    if (rightWord && downWord) {
        separatorLine->setVisible(true);
    }

    update();
}

void CrosswordCellWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(QBrush(Qt::black));


    int arrowHeadSize = ARROW_MARGING /2;
    int initialLineLength = ARROW_MARGING /2;


    if (currentRightWord)
    {
        int startingHeight = currentDownWord != nullptr ? (height()-ARROW_MARGING)  / 4: (height()-ARROW_MARGING) / 2;

        QPoint startPoint(width() - ARROW_MARGING, startingHeight);
        QPoint endOfLine = QPoint(startPoint.x() + initialLineLength, startingHeight);
        painter.drawLine(startPoint, endOfLine);

        if (currentRightWord->direction == Direction::Horizontal)
        {
            QPolygonF arrowHead;
            arrowHead << QPoint(width(),  startingHeight)
                      << QPoint(endOfLine.x(), startingHeight - arrowHeadSize / 2)
                      << QPoint(endOfLine.x(), startingHeight + arrowHeadSize / 2);
            painter.drawPolygon(arrowHead);

        }
        else if (currentRightWord->direction == Direction::Vertical)
        {
            QPoint bendPoint(startPoint.x() + initialLineLength, startingHeight + initialLineLength);
            painter.drawLine(endOfLine, bendPoint);

            QPolygonF arrowHead;
            arrowHead << QPoint(startPoint.x() + initialLineLength,  startingHeight + initialLineLength + arrowHeadSize)
                      << QPoint(startPoint.x() + initialLineLength - arrowHeadSize / 2, startingHeight + initialLineLength )
                      << QPoint(startPoint.x() + initialLineLength + arrowHeadSize / 2, startingHeight + initialLineLength);
            painter.drawPolygon(arrowHead);
        }
    }

    if (currentDownWord)
    {
        QPoint startPoint(width() / 2 , height() - ARROW_MARGING);
        QPoint endOfLine = QPoint(width() / 2 , height() - ARROW_MARGING / 2);
        painter.drawLine(startPoint, endOfLine);

        if (currentDownWord->direction == Direction::Vertical)
        {
            QPolygonF arrowHead;
            arrowHead << QPoint(width() / 2,  height())
                      << QPoint(width() / 2 - arrowHeadSize / 2, height() - ARROW_MARGING / 2)
                      << QPoint(width() / 2 + arrowHeadSize / 2 , height() - ARROW_MARGING / 2);
            painter.drawPolygon(arrowHead);

        }
        else if (currentDownWord->direction == Direction::Horizontal)
        {
            QPoint bendPoint(width() / 2 + ARROW_MARGING / 2, height() - ARROW_MARGING / 2);
            painter.drawLine(endOfLine, bendPoint);

            QPolygonF arrowHead;
            arrowHead << QPoint(width() / 2 + ARROW_MARGING,  height() - ARROW_MARGING / 2)
                      << QPoint(width() / 2 + ARROW_MARGING / 2, height() - ARROW_MARGING / 2 - arrowHeadSize / 2)
                      << QPoint(width() / 2 + ARROW_MARGING / 2, height() - ARROW_MARGING / 2 + arrowHeadSize / 2);
            painter.drawPolygon(arrowHead);
        }
    }
}