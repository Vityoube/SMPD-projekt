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

float MainWindow::matrixDeterminant(boost::numeric::ublas::matrix<float> matrix)
{
    boost::numeric::ublas::permutation_matrix<std::size_t> pm(matrix.size1());
    float determinant = 1.0;
    if (boost::numeric::ublas::lu_factorize(matrix, pm))
        determinant=0.0;
    else
        for(int i=0;i<matrix.size1();++i){
            determinant*=matrix(i,i);
         determinant*=matrixDeterminantSign(pm);
    }
    return determinant;
}

int MainWindow::matrixDeterminantSign(const boost::numeric::ublas::permutation_matrix<std::size_t> pemutationMatrix)
{
    int pmSign;
    std::size_t size = pemutationMatrix.size();
    for (std::size_t i=0;i<size;++i){
        if (i!=pemutationMatrix(i))
            pmSign*=-1;
    }
    return pmSign;
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
                int currentObjectNumber=0;
                std::cout<<"W bazie są "<<database.getNoObjects()<<" obiektów"<<std::endl;
                for (int j = 0; j<(database.getNoObjects());++j)
                {
                    Object ob=database.getObjectByIndex(j);
                    classAverages[ob.getClassName()] += (float)ob.getFeature(i);
                    classStds[ob.getClassName()] +=  (float)ob.getFeature(i) *  (float)ob.getFeature(i);
                    float currentClassStandartDeviation=classAverages[ob.getClassName()];
                    float currentClassAverage=classStds[ob.getClassName()];
                    std::cout<<"I\'m in average and standard deviation calculation. Current Object number: "<<currentObjectNumber++<<std::endl;
                    std::cout<<"Obiekt "<<j<<" należy do klasy "<<ob.getClassName()<<std::endl;
                    std::cout<<"Wartość cechy "<<i<<" dla obiekta "<<currentObjectNumber<<" wynosi: "<<(float)(ob.getFeature(i))<<std::endl;
//                    std::cout<<"Średnia dla bieżacej klasy:"<<currentClassStandartDeviation<<std::endl;
//                    std::cout<<"Odchylenie standartowe dla bieżącej klasy: "<<currentClassAverage<<std::endl;

                }
                std::cout<<"Średnia dla klasy A: "<<classAverages["Acer"]<<std::endl<<"Średnia dla klasy B: "<<classAverages["Quercus"]<<std::endl
                        <<"Odchylenie standartowe dla klasy A: "<<classStds["Acer"]<<std::endl<<"Odchylenie standartowe dla klasy B: "<<classStds["Quercus"]
                       <<std::endl;

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
          } else if (dimension>1){
            float FND=0, tmp=0;
            boost::numeric::ublas::matrix<float> sA(dimension,dimension);
            boost::numeric::ublas::matrix<float> sB(dimension,dimension);
            std::vector<int> maxIndexes;

            std::vector<std::vector<int>> indexesCombinationsVector;
            std::vector<bool> areIndexesPermited(database.getNoFeatures());
            std::fill(areIndexesPermited.begin(),areIndexesPermited.begin()+dimension,true);

            do {
                std::vector<int> currentIndexes;
                for (int i=0;i<database.getNoFeatures();++i){
                    if (areIndexesPermited[i])
                        currentIndexes.push_back(i);
                }
                indexesCombinationsVector.push_back(currentIndexes);
            } while(std::prev_permutation(areIndexesPermited.begin(),areIndexesPermited.end()));

//            int currentVector=0;
//            for (std::vector<int> currentIndexes :indexesCombiantionsVector){
//                std::cout<<"Vector #"<<currentVector<<" elements: ";
//                for (int i=0; i<currentIndexes.size();++i){
//                    if (i!=currentIndexes.size()-1)
//                        std::cout<<currentIndexes.at(i)<<", ";
//                    else
//                        std::cout<<currentIndexes.at(i);
//                }
//                std::cout<<std::endl;
//                currentVector++;
//            }
            boost::numeric::ublas::vector<float> dispersionA(dimension);
            boost::numeric::ublas::vector<float> dispersionB(dimension);
            boost::numeric::ublas::vector<float> meansA(dimension);
            boost::numeric::ublas::vector<float> meansB(dimension);
            for (int i=0;i<indexesCombinationsVector.size();++i){
                std::vector<int> currentFeatures=indexesCombinationsVector.at(i);
                for (int j=0;j<dimension;++j){
                    int currentFeature=currentFeatures.at(j);
                    int aObjectsCount=0, bObjectsCount=0;
                    for (Object currentObject: database.getObjects()){
                        if (currentObject.compareName("Acer")){
                            meansA[j]+=currentObject.getFeature(currentFeature);
                            aObjectsCount++;
                        } else if (currentObject.compareName("Quercus")){
                            meansB[j]+=currentObject.getFeature(currentFeature);
                            bObjectsCount++;
                        }
                    }
                    meansA[j]/=aObjectsCount;
                    meansB[j]/=bObjectsCount;
                    for (Object currentObjectForDispertion : database.getObjects()){
                        if (currentObjectForDispertion.compareName("Acer")){
                            dispersionA[j]+=(currentObjectForDispertion.getFeature(currentFeature)-meansA[j])*
                                    (currentObjectForDispertion.getFeature(currentFeature)-meansA[j]);
                        } else if (currentObjectForDispertion.compareName("Quercus")){
                            dispersionB[j]+=(currentObjectForDispertion.getFeature(currentFeature)-meansB[j])*
                                    (currentObjectForDispertion.getFeature(currentFeature)-meansB[j]);
                        }
                    }
                    dispersionA[j]=sqrt(dispersionA[j]);
                    dispersionB[j]=sqrt(dispersionB[j]);

//                    std::cout<<"Rorzut dla cechy #"<<currentFeature<<" dla klasy A = "<<dispersionA[i]<<std::endl;
//                    std::cout<<"Rorzut dla cechy #"<<currentFeature<<" dla klasy B = "<<dispersionB[i]<<std::endl;
                }
                for (int j=0;j<dimension;++j){
                    for (int k=0;k<dimension;++k){
                        sA(j,k)=dispersionA[j]*dispersionA[k];
                        sB(j,k)=dispersionB[j]*dispersionB[k];
                    }
                }
//                std::cout<<"Macierz rorzutu dla klasy A:"<<std::endl;
//                for (int j=0;j<dimension;++j){
//                    for (int k=0;k<dimension;++k){
//                        std::cout<<sA(j,k)<<", ";
//                    }
//                    std::cout<<std::endl;
//                }
//                std::cout<<std::endl;

//                std::cout<<"Macierz rorzutu dla klasy B:"<<std::endl;
//                for (int j=0;j<dimension;++j){
//                    for (int k=0;k<dimension;++k){
//                        std::cout<<sB(j,k)<<", ";
//                    }
//                    std::cout<<std::endl;
//                }
//                std::cout<<std::endl;
                float dispersionBetweenClasses=0;
                for (int j=0;j<dimension;++j){
                    dispersionBetweenClasses+=(meansA[j]-meansB[j])*(meansA[j]-meansB[j]);
                }
                dispersionBetweenClasses=sqrt(dispersionBetweenClasses);
                boost::numeric::ublas::matrix<float> s(dimension,dimension);
                s=sA+sB;
//                for (int j=0; j<dimension;++j)
//                    for(int k=0;k<dimension;++k)
//                        s(j,k)=sA(j,k)+sB(j,k);

                float dispersionInClasses=matrixDeterminant(s);
                tmp=dispersionBetweenClasses/dispersionInClasses;
                std::cout<<"Fisher for current features set: "<<tmp<<std::endl;
                if (tmp>FND && std::to_string(tmp).compare("inf")!=0){
                    FND=tmp;
                    maxIndexes=currentFeatures;
//                    std::cout<<"Maximum fisher value: "<<FND<<std::endl;
                }
            }
            std::string maxIndexesString="";
            for (int i=0;i<maxIndexes.size();++i){
                if (i<maxIndexes.size()-1)
                    maxIndexesString+=std::to_string(maxIndexes.at(i))+", ";
                else
                    maxIndexesString+=std::to_string(maxIndexes.at(i));
            }
            double fndDouble=(double)FND;
            std::cout<<"Maximum Fisher value: "<<FND<<std::endl;
            ui->FStextBrowserDatabaseInfo->append("Optimum features indexes: "+QString::fromStdString(maxIndexesString));
            ui->FStextBrowserDatabaseInfo->append("Maximum Fisher: "+QString::number(fndDouble));


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
        ui->CcomboBoxClassifiers->clear();
        ui->CcomboBoxK->clear();
        ui->CtextBrowser->append("Training part: " + QString::number(database.getNoTrainingObjects()));
        ui->CtextBrowser->append("Test part: " + QString::number(database.getNoTestObjects()));
        ui->CcomboBoxClassifiers->addItem("Nearest Neighbor (NN)");
        ui->CcomboBoxClassifiers->addItem("Nearest Mean (NM)");
        ui->CpushButtonExecute->setEnabled(true);
        if (database.getClassAObjectsCount()<=database.getClassBObjectsCount()){
            if (database.getClassAObjectsCount()%2==0)
                database.setK(database.getClassAObjectsCount()-1);
            else
                database.setK(database.getClassAObjectsCount());
        } else if (database.getClassAObjectsCount()>database.getClassBObjectsCount()){
            if (database.getClassBObjectsCount()%2==0)
                database.setK(database.getClassBObjectsCount()-1);
            else
                database.setK(database.getClassBObjectsCount());
        }
        int k=database.getK();
        for (int i=0;i<=k;i++){
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
        if (database.getK()%2!=0){
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
