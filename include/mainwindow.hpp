#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout> 


class GameInterface;
class GridGenerationInterface;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPlayButtonClicked();
    void onGenerateButtonClicked();

private:
    QPushButton *playButton;
    QPushButton *generateButton;

    GameInterface *gameInterface;
    GridGenerationInterface *gridGenerationInterface;
};

#endif // MAINWINDOW_H