#ifndef GRIDGENERATIONINTERFACE_H
#define GRIDGENERATIONINTERFACE_H

#include <QWidget>
#include <QSpinBox> 
#include <QLineEdit>
#include <QPushButton> 
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>
#include <QMessageBox>

class GridGenerator;

class GridGenerationInterface : public QWidget
{
    Q_OBJECT

public:
    explicit GridGenerationInterface(QWidget *parent = nullptr);
    ~GridGenerationInterface();

    void setAlgorithmOptions();
    void setHeuristicOptions();
    void setControlsEnabled(bool enable);

signals:

    void startGenerationRequested(
        int gridCols,
        int gridRows,
        const QString& dbPath,
        int poolDurationMs,
        int workerDurationMs,
        int numWorkers,
        const QString& outputDir,
        const QString& solvingAlgo,
        const QString& heuristic
    );


    //void stopGenerationRequested();
    void windowClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onBrowseDbPathButtonClicked();
    void onBrowseOutputFolderButtonClicked();
    void onStartGenerationButtonClicked();
    void onStopGenerationButtonClicked();


    void onGenerationProgress(int successCount, int failCount);
    void onAllGenerationsFinished();
    void onTotalGenerationProgress(int current);

private:
    QSpinBox *gridColsSpinBox;
    QSpinBox *gridRowsSpinBox;
    QLineEdit *dbPathLineEdit;
    QPushButton *browseDbButton;
    QSpinBox *poolDurationSpinBox;
    QSpinBox *workerDurationSpinBox;
    QSpinBox *numWorkersSpinBox;
    QLineEdit *outputFolderLineEdit;
    QPushButton *browseOutputButton;
    QComboBox *solvingAlgoComboBox;
    QComboBox *heuristicComboBox;


    QPushButton *startButton;
    QPushButton *stopButton;


    QLabel *statusLabel;
    QLabel *successCountLabel;
    QLabel *failCountLabel;
    QProgressBar *progressBar;

    GridGenerator *gridGenerator;

};

#endif // GRIDGENERATIONINTERFACE_H