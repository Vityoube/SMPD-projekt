#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <fstream>
#include <QCheckBox>
#include <QFileDialog>
#include <QtCore>
#include <QtGui>
#include <QMessageBox>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include"database.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    bool loadFile(const std::string &fileName);
    void updateDatabaseInfo();
    void saveFile(const std::string &fileName);

    void FSupdateButtonState(void);
    void FSsetButtonState(bool state);

    void CUpdateButtonState(void);
    void  CsetButtonState(bool state);

    float matrixDeterminant(boost::numeric::ublas::matrix<float> &matrix);

    int matrixDeterminantSign(const boost::numeric::ublas::permutation_matrix<std::size_t> & pemutationMatrix);
    int determinant_sign(const boost::numeric::ublas::permutation_matrix<std::size_t>& pm);
    double determinant(boost::numeric::ublas::matrix<float>& m);
    void PLoadDatabase(QString folderName);

private slots:
    void on_FSpushButtonOpenFile_clicked();

    void on_FSpushButtonCompute_clicked();

    void on_FSpushButtonSaveFile_clicked();

    void on_PpushButtonSelectFolder_clicked();


    void on_CpushButtonOpenFile_clicked();

    void on_CpushButtonSaveFile_clicked();

    void on_CpushButtonTrain_clicked();

    void on_CpushButtonExecute_clicked();

private:
    Ui::MainWindow *ui;

private:
     Database database;
     QString fileFolder="";
     QString imageFolder="";
};

#endif // MAINWINDOW_H
