#ifndef GAMEINTERFACE_HPP
#define GAMEINTERFACE_HPP

#include <QWidget>
#include <QLabel>

class GameInterface : public QWidget
{
    Q_OBJECT
public:
    explicit GameInterface(QWidget *parent = nullptr);

signals:
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // GAMEINTERFACE_HPP