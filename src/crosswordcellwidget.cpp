#include "crosswordcellwidget.hpp"
#include <QPainter>
#include <QDebug>

CrosswordCellWidget::CrosswordCellWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(50, 50);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2, 2, 2, 2);
    mainLayout->setSpacing(0);

    setStyleSheet(
        "background-color: lightgray; "
        "border: 1px solid black; "
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
    painter.setPen(QPen(Qt::darkRed, 2));

    // ... (Votre code pour dessiner les flèches reste le même)
    // Exemple très simplifié pour une flèche horizontale à droite
    if (currentRightWord && currentRightWord->direction == Direction::Horizontal) {
        int arrowSize = 5;
        QPoint tip = QPoint(width() - 1, rect().center().y());
        painter.drawLine(tip - QPoint(arrowSize, arrowSize), tip);
        painter.drawLine(tip - QPoint(arrowSize, -arrowSize), tip);
    }

    // Exemple très simplifié pour une flèche verticale vers le bas
    if (currentDownWord && currentDownWord->direction == Direction::Vertical) {
        int arrowSize = 5;
        QPoint tip = QPoint(rect().center().x(), height() - 1);
        painter.drawLine(tip - QPoint(arrowSize, arrowSize), tip);
        painter.drawLine(tip - QPoint(-arrowSize, arrowSize), tip);
    }
}