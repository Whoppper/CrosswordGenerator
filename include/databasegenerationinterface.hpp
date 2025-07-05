#ifndef DATABASEGENERATIONINTERFACE_HPP
#define DATABASEGENERATIONINTERFACE_HPP

#include <QWidget>
#include <QString>

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;

class DatabaseGenerationInterface : public QWidget
{
    Q_OBJECT

public:
    explicit DatabaseGenerationInterface(QWidget *parent = nullptr);
    ~DatabaseGenerationInterface();

signals:
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onSelectInputFileClicked();
    void onSelectOutputFolderClicked();
    void onGenerateDatabaseClicked();

private:
    QLineEdit *inputFilePathEdit;
    QLineEdit *outputFolderPathEdit;
    QPushButton *selectInputFileButton;
    QPushButton *selectOutputFolderButton;
    QPushButton *generateDatabaseButton;

    void generateDatabase();
};

#endif