#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include <QLineEdit> 
#include <QMap>
#include <QScrollArea>

#include "crosswordmanager.hpp"
#include "crosswordcell.hpp" 
#include "logger.hpp"

inline bool operator<(const QPoint& p1, const QPoint& p2) {
    if (p1.x() != p2.x()) {
        return p1.x() < p2.x();
    }
    return p1.y() < p2.y();
}

class GameInterface : public QWidget
{
    Q_OBJECT

public:
    explicit GameInterface(QWidget *parent = nullptr);
    ~GameInterface();

signals:
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onLoadGridButtonClicked();
    void loadGridFromJson(const QString& filePath);
    void displayGrid(bool showSolution = false);
    void onCellEdited(const QString &text);

private:
    QVBoxLayout *mainLayout;
    QPushButton *loadGridButton;
    QGridLayout *gridDisplayLayout;
    QPushButton *showSolutionButton;
    QLabel *statusLabel;
    
    CrosswordManager *gameCrosswordManager;
    QMap<QPoint, QLineEdit*> cellInputWidgets;
    QScrollArea *scrollArea;
    QWidget* gridContainer;
};

#endif // GAMEINTERFACE_H