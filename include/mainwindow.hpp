#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout> 


class GameInterface;
class GridGenerationInterface;
class DatabaseGenerationInterface;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPlayButtonClicked();
    void onGenerateButtonClicked();
    void onGenerateDatabaseButtonClicked();

private:
    QPushButton *playButton;
    QPushButton *generateButton;
    QPushButton *generateDatabaseButton;

    GameInterface *gameInterface;
    GridGenerationInterface *gridGenerationInterface;
    DatabaseGenerationInterface *databaseGenerationInterface;
};

#endif // MAINWINDOW_H