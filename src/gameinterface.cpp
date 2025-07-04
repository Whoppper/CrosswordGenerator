#include "gameinterface.hpp"
#include <QVBoxLayout>

GameInterface::GameInterface(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Jouer une Partie");
    resize(800, 600); 

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("Bienvenue sur l'interface de jeu !", this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}

void GameInterface::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    QWidget::closeEvent(event);
}