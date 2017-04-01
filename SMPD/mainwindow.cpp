#include "mainwindow.h"
#include "ui_mainwindow.h"



#include <QImage>
#include <QDebug>




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QDoubleValidator* validator=new QDoubleValidator(0.00127551,0.99872449,10,ui->CTrainPartLineEdit);
    validator->setNotation(QDoubleValidator::StandardNotation);

    FSupdateButtonState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateDatabaseInfo()
{
    ui->FScomboBox->clear();
    for(unsigned int i=1; i<=database.getNoFeatures(); ++i)
        ui->FScomboBox->addItem(QString::number(i));

    ui->FStextBrowserDatabaseInfo->setText("noClass: " +  QString::number(database.getNoClass()));
    ui->FStextBrowserDatabaseInfo->append("noObjects: "  +  QString::number(database.getNoObjects()));
    ui->FStextBrowserDatabaseInfo->append("noFeatures: "  +  QString::number(database.getNoFeatures()));

}

void MainWindow::FSupdateButtonState(void)
{
    if(database.getNoObjects()==0)
    {
        FSsetButtonState(false);
    }
    else
        FSsetButtonState(true);

}


void MainWindow::FSsetButtonState(bool state)
{
   ui->FScomboBox->setEnabled(state);
   ui->FSpushButtonCompute->setEnabled(state);
   ui->FSpushButtonSaveFile->setEnabled(state);
   ui->FSradioButtonFisher->setEnabled(state);
   ui->FSradioButtonSFS->setEnabled(state);
}

void MainWindow::CUpdateButtonState()
{
    if (database.getNoObjects()==0)
        CsetButtonState(false);
    else
        CsetButtonState(true);
}

void MainWindow::CsetButtonState(bool state)
{
    ui->CpushButtonTrain->setEnabled(state);
    ui->CpushButtonSaveFile->setEnabled(state);
}

void MainWindow::on_FSpushButtonOpenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open TextFile"), fileFolder, tr("Texts Files (*.txt)"));

    if ( !database.load(fileName.toStdString()) )
        QMessageBox::warning(this, "Warning", "File corrupted !!!");
    else
        QMessageBox::information(this, fileName, "File loaded !!!");

    FSupdateButtonState();
    updateDatabaseInfo();
}

void MainWindow::on_FSpushButtonCompute_clicked()
{
    int dimension = ui->FScomboBox->currentText().toInt();


    if( ui->FSradioButtonFisher ->isChecked())
    {
    if (dimension == 1 && database.getNoClass() == 2)
        {
            float FLD = 0, tmp;
            int max_ind = -1;

            //std::map<std::string, int> classNames = database.getClassNames();
            for (uint i = 0; i < database.getNoFeatures(); ++i)
            {
                std::map<std::string, float> classAverages;
                std::map<std::string, float> classStds;

                for (auto const &ob : database.getObjects())
                {
                    classAverages[ob.getClassName()] += ob.getFeatures()[i];
                    classStds[ob.getClassName()] += ob.getFeatures()[i] * ob.getFeatures()[i];
                }

                std::for_each(database.getClassCounters().begin(), database.getClassCounters().end(), [&](const std::pair<std::string, int> &it)
                {
                    classAverages[it.first] /= it.second;
                    classStds[it.first] = std::sqrt(classStds[it.first] / it.second - classAverages[it.first] * classAverages[it.first]);
                }
                );

                tmp = std::abs(classAverages[ database.getClassNames()[0] ] - classAverages[database.getClassNames()[1]]) / (classStds[database.getClassNames()[0]] + classStds[database.getClassNames()[1]]);

                if (tmp > FLD)
                {
                    FLD = tmp;
                    max_ind = i;
                }

              }

            ui->FStextBrowserDatabaseInfo->append("max_ind: "  +  QString::number(max_ind) + " " + QString::number(FLD));
          }
     }
}



void MainWindow::on_FSpushButtonSaveFile_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
    tr("Open TextFile"), "maple_oak_results.txt", tr("Texts Files (*.txt)"));

        QMessageBox::information(this, "My File", fileName);
        database.save(fileName.toStdString());
}

void MainWindow::on_PpushButtonSelectFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    fileFolder=dir;
}

void MainWindow::on_CpushButtonOpenFile_clicked()
{
    database.clearObjects();
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open TextFile"), fileFolder, tr("Texts Files (*.txt)"));

    if ( !database.load(fileName.toStdString()) )
        QMessageBox::warning(this, "Warning", "File corrupted !!!");
    else
        QMessageBox::information(this, fileName, "File loaded !!!");

    ui->CtextBrowser->append("noClass: " +  QString::number(database.getNoClass()));
    ui->CtextBrowser->append("noObjects: "  +  QString::number(database.getNoObjects()));
    ui->CtextBrowser->append("noFeatures: "  +  QString::number(database.getNoFeatures()));
    ui->CpushButtonTrain->setEnabled(true);
    ui->CpushButtonSaveFile->setEnabled(true);
    ui->CpushButtonExecute->setEnabled(false);
    ui->CcomboBoxClassifiers->clear();
    ui->CcomboBoxK->clear();

}

void MainWindow::on_CpushButtonSaveFile_clicked()
{

}

void MainWindow::on_CpushButtonTrain_clicked()
{

    if (ui->CTrainPartLineEdit->text().isEmpty()){
        QMessageBox errorMessage;
        errorMessage.setText("Please enter training part percentage!");
        errorMessage.exec();
    }
    else if(database.trainObjects(ui->CTrainPartLineEdit->text().toDouble()/100)){
        ui->CtextBrowser->clear();
        ui->CcomboBoxK->clear();
        ui->CtextBrowser->append("Training part: " + QString::number(database.getNoTrainingObjects()));
        ui->CtextBrowser->append("Test part: " + QString::number(database.getNoTestObjects()));
        ui->CcomboBoxClassifiers->addItem("Nearest Neighbor (NN)");
        ui->CcomboBoxClassifiers->addItem("Nearest Mean (NM)");
        ui->CpushButtonExecute->setEnabled(true);
        if (database.getNoTrainingObjects()<=database.getNoTestObjects()){
            if (database.getNoTrainingObjects()%2==0)
                database.setK(database.getNoTrainingObjects()-1);
            else
                database.setK(database.getNoTrainingObjects());
        } else if (database.getNoTrainingObjects()>database.getNoTestObjects()){
            if (database.getNoTestObjects()%2==0)
                database.setK(database.getNoTestObjects()-1);
            else
                database.setK(database.getNoTestObjects());
        }
        int k=database.getK();
        for (int i=0;i<=k;i+=2){
            ui->CcomboBoxK->addItem(QString::number(i+1));
        }

    } else {
        QMessageBox errorMessage;
        if (database.getObjects().empty())
            errorMessage.setText("Objects database is empty!");
        else
            errorMessage.setText("Too much objects for training part!");
        errorMessage.exec();
    }
}

void MainWindow::on_CpushButtonExecute_clicked()
{
    if (ui->CcomboBoxClassifiers->currentText().toStdString().compare("Nearest Neighbor (NN)")==0){
        database.setK(ui->CcomboBoxK->currentText().toInt());
        double probability=database.classifyNN();
        ui->CtextBrowser->setFocus();
        QTextCursor cursor=ui->CtextBrowser->textCursor();
        ui->CtextBrowser->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
        ui->CtextBrowser->moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        ui->CtextBrowser->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
        if (ui->CtextBrowser->textCursor().selectedText().contains("Probability")){
            ui->CtextBrowser->textCursor().removeSelectedText();
            ui->CtextBrowser->textCursor().deletePreviousChar();
        }
        ui->CtextBrowser->setTextCursor(cursor);
        ui->CtextBrowser->append("Probability: "+QString::number(probability)+"%");
    }
    if (ui->CcomboBoxClassifiers->currentText().toStdString().compare("Nearest Mean (NM)")==0){
        database.setK(ui->CcomboBoxK->currentText().toInt());
        double probability=database.classifyNM();
        ui->CtextBrowser->setFocus();
        QTextCursor cursor=ui->CtextBrowser->textCursor();
        ui->CtextBrowser->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
        ui->CtextBrowser->moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        ui->CtextBrowser->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
        if (ui->CtextBrowser->textCursor().selectedText().contains("Probability")){
            ui->CtextBrowser->textCursor().removeSelectedText();
            ui->CtextBrowser->textCursor().deletePreviousChar();
        }
        ui->CtextBrowser->setTextCursor(cursor);
        ui->CtextBrowser->append("Probability: "+QString::number(probability)+"%");
    }
}
