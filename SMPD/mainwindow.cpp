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

int MainWindow::determinant_sign(const boost::numeric::ublas::permutation_matrix<std::size_t>& pm)
{
        int pm_sign = 1;
        std::size_t size = pm.size();
        for (std::size_t i = 0; i < size; ++i)
                if (i != pm(i))
                        pm_sign *= -1.0; // swap_rows would swap a pair of rows here, so wechange sign
        return pm_sign;
}

double MainWindow::determinant(boost::numeric::ublas::matrix<float>& m)
{
        boost::numeric::ublas::permutation_matrix<std::size_t> pm(m.size1());
        double det = 1.0;
        if (boost::numeric::ublas::lu_factorize(m, pm)) {
                det = 0.0;
        }
        else {
                for (int i = 0; i < m.size1(); i++)
                        det *= m(i, i); // multiply by elements on diagonal
                det = det * determinant_sign(pm);
        }
        return det;
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



float MainWindow::matrixDeterminant( boost::numeric::ublas::matrix<float>& m ) {
    float det = 1.0;
    if (m.size1()>2 && m.size2()>2){
        for (int i=0;i<m.size1();i++){
            for (int j=0;j<m.size2();j++){
                float a=m(i,j);
                int sign=pow(-1,i+j);
                std::vector<float> temporalMinorMatrix;
                boost::numeric::ublas::matrix<float> minorMatrix(m.size1()-1,m.size2()-1);
//                std::cout<<"Current element "<<i<<", "<<j<<"="<<a<<std::endl;
                for(int k=0;k<m.size1();k++){
                    for (int z=0;z<m.size2();z++){
                        temporalMinorMatrix.push_back(m(k,z));
//                        std::cout<<"Matrix element "<<k<<", "<<z<< " to minor: "<<m(k,z)<<std::endl;
                        if (k==i)
                            temporalMinorMatrix.pop_back();
                        if (z==j && k!=i){
                            temporalMinorMatrix.pop_back();
                        }
                    }
                }
//                std::cout<<"Temporal minor matrix: ";
//                for (float element : temporalMinorMatrix)
//                    std::cout<<element<<", ";
//                std::cout<<std::endl;
                int mK=0, mZ=0;
//                std::cout<<"Temporal minor matrix size "<<temporalMinorMatrix.size()<<std::endl;
//                std::cout<<"Minor matrix "<<minorMatrix.size1()<<" x "<<minorMatrix.size2()<<std::endl;
                for (int k=0;k<temporalMinorMatrix.size();k++){
                    if ((k)%minorMatrix.size2()!=0 || k==0){
                        minorMatrix(mK,mZ)=temporalMinorMatrix.at(k);
//                        std::cout<<"mK: "<<mK<<" mZ: "<<mZ<<" elementValue: "<<minorMatrix(mK,mZ)<<std::endl;
                        ++mZ;
                    } else if ((k)%(minorMatrix.size1()*minorMatrix.size2())==0 && (k)%minorMatrix.size2()!=0){
                        minorMatrix(mK,mZ)=temporalMinorMatrix.at(k);
//                        std::cout<<"mK: "<<mK<<" mZ: "<<mZ<<" elementValue: "<<minorMatrix(mK,mZ)<<std::endl;

                    }else{
                        mK++;
                        mZ=0;
                        minorMatrix(mK,mZ)=temporalMinorMatrix.at(k);
//                        std::cout<<"mK: "<<mK<<" mZ: "<<mZ<<" elementValue: "<<minorMatrix(mK,mZ)<<std::endl;
                        mZ++;
                    }
                }
//                std::cout<<"I'm here!"<<std::endl;
                det=sign*a*matrixDeterminant(minorMatrix);
            }
        }
    } else if (m.size1()==2 && m.size2()==2){
        det=m(0,0)*m(1,1)-m(0,1)*m(1,0);
//        std::cout<<"Matrix: \n"<<m(0,0)<<", "<<m(0,1)<<"\n"<<m(1,0)<<", "<<m(1,1)<<std::endl;
    } else if (m.size1()==1 && m.size2()==1){
        det=m(0,0);

    }
//    if (m.size1()>2)
//        std::cout<<"Determinant of matrix: "<<det<<std::endl;
    return det;
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

            std::vector<int> maxIndexes;

            std::vector<std::vector<int>> indexesCombinationsVector;
            std::vector<bool> areIndexesPermited(database.getNoFeatures());
            std::fill(areIndexesPermited.begin(),areIndexesPermited.begin()+dimension,true);
            std::vector<Object> classA, classB;
            for (Object currentObject: database.getObjects()){
                if (currentObject.compareName("Acer"))
                    classA.push_back(currentObject);
                else if (currentObject.compareName("Quercus"))
                    classB.push_back(currentObject);
            }
            int classACount=classA.size(), classBCount=classB.size();
            do {
                std::vector<int> currentIndexes;
                for (int i=0;i<database.getNoFeatures();++i){
                    if (areIndexesPermited[i])
                        currentIndexes.push_back(i);
                }
                indexesCombinationsVector.push_back(currentIndexes);
            } while(std::prev_permutation(areIndexesPermited.begin(),areIndexesPermited.end()));
            for (int i=0;i<indexesCombinationsVector.size();++i){

                boost::numeric::ublas::matrix<float> dispersionA(dimension,classACount);
                boost::numeric::ublas::matrix<float> dispersionB(dimension,classBCount);
                boost::numeric::ublas::vector<float> meansA(dimension);
                boost::numeric::ublas::vector<float> meansB(dimension);
                for (int j=0;j<dispersionA.size1();j++){
                    for (int k=0;k<dispersionA.size2();k++){
                        dispersionA(j,k)=0.0f;
                    }
                }
                for (int j=0;j<dispersionB.size1();j++){
                    for (int k=0;k<dispersionB.size2();k++){
                        dispersionB(j,k)=0.0f;
                    }
                }
                for (int j=0;j<meansA.size();j++){
                        meansA(j)=0.0f;
                }
                for (int j=0;j<meansB.size();j++){
                        meansB(j)=0.0f;
                }
                std::vector<int> currentFeatures=indexesCombinationsVector.at(i);
                for (int j=0;j<dimension;++j){
                    int currentFeature=currentFeatures.at(j);
                    int aObjectsCount=0, bObjectsCount=0;
                    for (Object classAObject : classA){
                        meansA(j)+=classAObject.getFeature(currentFeature);
                    }
                    for (Object classBObject : classB){
                        meansB(j)+=classBObject.getFeature(currentFeature);
                    }
                    meansA(j)/=classACount;
                    meansB(j)/=classBCount;
                    for (int k=0;k<classACount;k++){
                        Object currentAObject=classA.at(k);
                        dispersionA(j,k)=currentAObject.getFeature(currentFeature)-meansA(j);
                    }
                    for (int k=0;k<classBCount;k++){
                        Object currentBObject=classB.at(k);
                        dispersionB(j,k)=currentBObject.getFeature(currentFeature)-meansB(j);
                    }
                }
                boost::numeric::ublas::matrix<float> dispersionATransposed=boost::numeric::ublas::trans(dispersionA);
                boost::numeric::ublas::matrix<float> dispersionBTransposed=boost::numeric::ublas::trans(dispersionB);

                boost::numeric::ublas::matrix<float> sA(dimension,dimension);
                boost::numeric::ublas::matrix<float> sB(dimension,dimension);
                for (int j=0;j<sA.size1();j++){
                    for (int z=0;z<sA.size2();z++){
                        sA(j,z)=0.0;
                        for( int z1=0;z1<dispersionA.size2();z1++){
                            sA(j,z)+=dispersionA(j,z1)*dispersionATransposed(z1,z);
                        }
                    }
                }
                for (int j=0;j<sB.size1();j++){
                    for (int z=0;z<sB.size2();z++){
                        sB(j,z)=0.0;
                        for( int z1=0;z1<dispersionB.size2();z1++){
                            sB(j,z)+=dispersionB(j,z1)*dispersionBTransposed(z1,z);
                        }
                    }
                }
                float dispersionBetweenClasses=0;
                for (int j=0;j<dimension;++j){
                    dispersionBetweenClasses+=(meansA(j)-meansB(j))*(meansA(j)-meansB(j));
                }                
                dispersionBetweenClasses=sqrt(dispersionBetweenClasses);
                std::cout<<"dispersion between classes in current set: "<<dispersionBetweenClasses<<std::endl;
                boost::numeric::ublas::matrix<float> s(dimension,dimension);
                s=sA+sB;
                float dispersionInClasses=determinant(s);
                std::cout<<"dispersion in classes in current set: "<<dispersionInClasses<<std::endl;
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
    } else if (ui->FSradioButtonSFS->isChecked()){
        std::vector<Object> classA, classB;
        for (Object currentObject: database.getObjects()){
            if (currentObject.compareName("Acer"))
                classA.push_back(currentObject);
            else if (currentObject.compareName("Quercus"))
                classB.push_back(currentObject);
        }
        int classACount=classA.size(), classBCount=classB.size();        
        std::vector<int> maxIndexes;
        boost::numeric::ublas::vector<float> meansA(dimension), meansB(dimension);
        boost::numeric::ublas::matrix<float> dispersionA(dimension,classACount), dispersionB(dimension,classBCount);
        float maxSFS=0, currentMaxSFS=0;
        int currentMaxIndex=-1;
        for (int featureCount=1;featureCount<=dimension;featureCount++){
            float currentMaxAMean=0.0f,currentMaxBMean=0.0f;
            currentMaxSFS=0;
            boost::numeric::ublas::matrix<float> currentDispersionA(dimension,classACount), currentDispersionB(dimension,classBCount);
            for (int currentFeature=0; currentFeature<database.getNoFeatures();currentFeature++){                
                if (std::find(maxIndexes.begin(),maxIndexes.end(),currentFeature)==maxIndexes.end()){
                    boost::numeric::ublas::vector<float> currentMeansA(featureCount), currentMeansB(featureCount);
                    if (featureCount>1){
                        for (int i=0;i<featureCount-1;i++){
                            currentMeansA(i)=meansA(i);
                            currentMeansB(i)=meansB(i);
                        }
                    }
                    currentMeansA(featureCount-1)=0.0;
                    currentMeansB(featureCount-1)=0.0;
                    boost::numeric::ublas::matrix<float> sA(featureCount,featureCount), sB(featureCount,featureCount);
                    for (int i=0;i<sA.size1();i++){
                        for (int j=0;j<sA.size2();j++){
                            sA(i,j)=0.0f;
                        }
                    }
                    for (int i=0;i<sB.size1();i++){
                        for (int j=0;j<sB.size2();j++){
                            sB(i,j)=0.0f;
                        }
                    }
                    for (Object classAObject : classA){
                        currentMeansA(featureCount-1)+=classAObject.getFeature(currentFeature);
                    }
                    for (Object classBObject : classB){
                        currentMeansB(featureCount-1)+=classBObject.getFeature(currentFeature);
                    }
                    currentMeansA(featureCount-1)/=classACount;
                    currentMeansB(featureCount-1)/=classBCount;
                    for( int j=0;j<featureCount;j++){
                        for (int i=0; i<classACount;i++){
                            Object currentAObject=classA.at(i);
                            currentDispersionA(j,i)=currentAObject.getFeature(currentFeature)-currentMeansA(j);
                        }
                    }
                    for( int j=0;j<featureCount;j++){
                        for (int i=0; i<classBCount;i++){
                            Object currentBObject=classB.at(i);
                            currentDispersionB(j,i)=currentBObject.getFeature(currentFeature)-currentMeansB(j);
                        }
                    }
//                    std::cout<<"Dispersion matrix A: "<<std::endl;
//                    for (int i=0;i<dispersionA.size1();i++){
//                        for (int j=0;j<dispersionA.size2();j++){
//                            std::cout<<dispersionA(i,j)<<", ";
//                        }
//                        std::cout<<std::endl;
//                    }
//                    std::cout<<"Dispersion matrix B: "<<std::endl;
//                    for (int i=0;i<dispersionB.size1();i++){
//                        for (int j=0;j<dispersionB.size2();j++){
//                            std::cout<<dispersionB(i,j)<<", ";
//                       }
//                       std::cout<<std::endl;
//                   }
                    boost::numeric::ublas::matrix<float> currentDispersionATransposed=boost::numeric::ublas::trans(currentDispersionA);
                    boost::numeric::ublas::matrix<float> currentDispersionBTransposed=boost::numeric::ublas::trans(currentDispersionB);
                    //                    std::cout<<"Dispersion matrix A transposed: "<<std::endl;
                    //                       for (int i=0;i<dispersionATransposed.size1();i++){
                    //                           for (int j=0;j<dispersionATransposed.size2();j++){
                    //                               std::cout<<dispersionATransposed(i,j)<<", ";
                    //                           }
                    //                           std::cout<<std::endl;
                    //                       }
                    //                       std::cout<<"Dispersion matrix B transposed: "<<std::endl;
                    //                       for (int i=0;i<dispersionBTransposed.size1();i++){
                    //                           for (int j=0;j<dispersionBTransposed.size2();j++){
                    //                               std::cout<<dispersionBTransposed(i,j)<<", ";
                    //                          }
                    //                          std::cout<<std::endl;
                    //                      }
                    for (int j=0;j<sA.size1();j++){
                        for (int z=0;z<sA.size2();z++){
                            for( int z1=0;z1<dispersionA.size2();z1++){
                                sA(j,z)+=currentDispersionA(j,z1)*currentDispersionATransposed(z1,z);
                            }
                        }
                    }
                    for (int j=0;j<sB.size1();j++){
                        for (int z=0;z<sB.size2();z++){
                            for( int z1=0;z1<dispersionB.size2();z1++){
                                sB(j,z)+=currentDispersionB(j,z1)*currentDispersionBTransposed(z1,z);
                            }
                        }
                    }
                    std::cout<<"sA matrix: "<<std::endl;
                    for (int i=0;i<sA.size1();i++){
                        for (int j=0;j<sA.size2();j++){
                            std::cout<<sA(i,j)<<", ";
                        }
                        std::cout<<std::endl;
                    }
                    std::cout<<"sB matrix: "<<std::endl;
                    for (int i=0;i<sB.size1();i++){
                        for (int j=0;j<sB.size2();j++){
                            std::cout<<sB(i,j)<<", ";
                        }
                        std::cout<<std::endl;
                    }
                    float dispersionBetweenClasses=0.0;
                    for (int i=0;i<featureCount;i++)
                        dispersionBetweenClasses+=(currentMeansA(i)-currentMeansB(i))*(currentMeansA(i)-currentMeansB(i));
                    dispersionBetweenClasses=sqrt(dispersionBetweenClasses);
                    boost::numeric::ublas::matrix<float> s(featureCount,featureCount);
                    s=sA+sB;
                    float dispersionInClasses=0.0;
                    dispersionInClasses=determinant(s);
                    float currentSFS=dispersionBetweenClasses/dispersionInClasses;
                    std::cout<<"dispersion in classes: "<<dispersionInClasses<<std::endl;
                    std::cout<<"dispersion between classes: "<<dispersionBetweenClasses<<std::endl;
                    std::cout<<"SFS value: "<<currentSFS<<std::endl;
                    if (currentSFS>currentMaxSFS   && std::to_string(currentSFS).compare("inf")!=0){
                        currentMaxSFS=currentSFS;
                        currentMaxIndex=currentFeature;
                        currentMaxAMean=currentMeansA(featureCount-1);
                        currentMaxBMean=currentMeansB(featureCount-1);
                    }
                }
            }

            if (currentMaxIndex!=-1)
                maxIndexes.push_back(currentMaxIndex);
            meansA(featureCount-1)=currentMaxAMean;
            meansB(featureCount-1)=currentMaxBMean;
            maxSFS=currentMaxSFS;
            currentMaxIndex=-1;

        }
//        std::cout<<"Max SFS value: "<<maxSFS<<std::endl;
        double doubleMaxSFS=(double)maxSFS;
        std::string maxIndexesString="";
        for (int i=0;i<maxIndexes.size();++i){
            if (i<maxIndexes.size()-1)
                maxIndexesString+=std::to_string(maxIndexes.at(i))+", ";
            else
                maxIndexesString+=std::to_string(maxIndexes.at(i));
        }
        ui->FStextBrowserDatabaseInfo->append("Optimum features indexes: "+QString::fromStdString(maxIndexesString));
        ui->FStextBrowserDatabaseInfo->append("Maximum SFS: "+QString::number(doubleMaxSFS));

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
    ui->CcomboBoxValidation->setEnabled(true);
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
    else if(database.trainObjects(ui->CTrainPartLineEdit->text().toDouble()/100,ui->CcomboBoxValidation->currentText().toStdString())){
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
