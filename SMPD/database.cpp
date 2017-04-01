#include "database.h"
#include <fstream>
#include <QDebug>

unsigned int Database::getNoTrainingObjects() const
{
    return noTrainingObjects;
}

unsigned int Database::getNoTestObjects() const
{
    return noTestObjects;
}

void Database::clearObjects()
{
    objects.clear();
    trainingObjects.clear();
    testObjects.clear();
    noFeatures=0;
    noObjects=0;
    noTestObjects=0;
    noTrainingObjects=0;
    featuresIDs.clear();
}

unsigned int Database::getK() const
{
    return k;
}

void Database::setK(unsigned int value)
{
    k = value;
}

std::map<Object, std::vector<Object> > Database::getNearestMeansForTestObjects() const
{
    return nearestMeansForTestObjects;
}

void Database::setNearestMeansForTestObjects(const std::map<Object, std::vector<Object> > &value)
{
    nearestMeansForTestObjects = value;
}

Database::Database() : noClass(0), noObjects(0), noFeatures(0)
{
}

bool Database::addObject(const Object &object)
{
    if (noFeatures == 0)
        noFeatures = object.getFeaturesNumber();
    else if (noFeatures != object.getFeaturesNumber())
        return false;

    objects.push_back(object);
    ++noObjects;

    if (classCounters[object.getClassName()]++ == 0)
        classNamesVector.push_back(object.getClassName());


    return true;
}

bool Database::load(const std::string &fileName)
{
    clear();

    std::ifstream file(fileName);

    if (!file.is_open())
    {
        return false;
    }
    std::string line; getline(file, line);

    size_t pos = line.find_first_of(',');
    if (pos == std::string::npos)
        return false;
    unsigned int classFeaturesNo = std::stoi(line.substr(0, pos));

    std::string featuresID = line.substr(pos + 1);

    while (true)
    {
        pos = featuresID.find_first_of(',');
        if (pos != std::string::npos)
        {
            std::string feature = featuresID.substr(0, pos);
            featuresID = featuresID.substr(pos + 1);
            unsigned int featureID = std::stof(feature);
            featuresIDs.push_back(featureID);
        }
        else
        {
            unsigned int featureID = std::stof(featuresID);
            featuresIDs.push_back(featureID);
            break;
        }
    }

    for (std::string line; getline(file, line);)
    {
        size_t pos = line.find_first_of(',');
        if (pos == std::string::npos)
        {
            // logowanie błędu przy odczycie z pliku
          continue;
        }

        std::string className = line.substr(0, pos);

        size_t classNamePos = className.find_first_of(' ');
        if (classNamePos != std::string::npos)
            className = className.substr(0, classNamePos);

        std::string features = line.substr(pos+1);

        std::vector<float> featuresValues;

        while (true)
        {

            pos = features.find_first_of(',');
            if (pos != std::string::npos)
            {
                std::string feature = features.substr(0, pos);
                features = features.substr(pos + 1);
                float featureValue = std::stof(feature);
                featuresValues.push_back(featureValue);
            }
            else
            {
                float featureValue = std::stof(features);
                featuresValues.push_back(featureValue);
                break;
            }
        }

        if(classFeaturesNo == featuresValues.size())
        {
            if(addObject(Object(className, featuresValues)))
            {
                // logowanie błędu przy dodawaniu obiektu do bazy
            }
        }
        else return false;

    }
    file.close(); // logowanie poprawnego wczytania do bazy
    return true;
}

void Database::save(const std::string &fileName)
{
    std::ofstream file(fileName, std::ios::out | std::ios::in | std::ios::trunc);

    file << getNoFeatures();

    for (auto const &id :  featuresIDs)
    {
        file << "," << id;
    }

    file << "\n";

    for (auto const &ob :  getObjects())
    {
        file << ob.getClassName() + ",";

        std::for_each(ob.getFeatures().begin(), ob.getFeatures().end(), [&](float n)
        {
            file << n << ",";

        }
        );

        file << "\n";
    }

    file.close();
}

bool Database::trainObjects(double trainingPartPercent)
{
    testObjects.clear();
    trainingObjects.clear();
    trainObjectsIds.clear();
    if (ceil(objects.size()*trainingPartPercent)<objects.size()){
        noTrainingObjects=objects.size()*trainingPartPercent;
        while(trainingObjects.size()<noTrainingObjects){
            unsigned int currentObjectId=rand()%noObjects;
            if (std::find(trainObjectsIds.begin(),trainObjectsIds.end(),currentObjectId)!=trainObjectsIds.end())
                continue;
            trainingObjects.push_back(objects.at(currentObjectId));
            trainObjectsIds.push_back(currentObjectId);
        }
        noTestObjects=objects.size()-noTrainingObjects;
        std::cout<<"objects size: "<<objects.size()<<std::endl;
        for (int i=0;i<objects.size();i++){
            Object object=objects.at(i);
            std::cout<<"current object: "<<object.getClassName()<<std::endl;
            if (std::find(trainObjectsIds.begin(),trainObjectsIds.end(),i)!=trainObjectsIds.end())
                continue;
            testObjects.push_back(object);
            std::cout<<"testObjects size: "<<testObjects.size()<<std::endl;
        }
        return true;
    }
    return false;

}

double Database::classifyNN()
{
    nearestNeighborsClassNamesForTestObjects.clear();
    std::vector<int> nearestNeighborsForTestObjectIds;
    double probability=0.0;
    std::cout<<"Before main loop. trainingObjects size: "<<trainingObjects.size()<<std::endl;
    for (Object testObject : testObjects){
        std::cout<<"In main loop"<<std::endl;
        float minDistance=99999.9f;        
        for (int i=0;i<k;i++){
            int nearestNeighborsForTestObjectId=10000;
            for (int z=0;z<trainingObjects.size();z++){
                Object trainingObject=trainingObjects.at(i);
                float distanceToTrainingObject=0.0f;
                unsigned int featuresNumber=trainingObject.getFeaturesNumber();
                for (unsigned int j=0;j<featuresNumber;j++){
                    distanceToTrainingObject+=((trainingObject.getFeature(j)-testObject.getFeature(j))*
                            (trainingObject.getFeature(j)-testObject.getFeature(j)));
                }
                distanceToTrainingObject=sqrt(distanceToTrainingObject);
                if (std::find(nearestNeighborsForTestObjectIds.begin(),nearestNeighborsForTestObjectIds.end(),z)!=nearestNeighborsForTestObjectIds.end()){
                    std::cout<<"Found object already in neighbors list"<<std::endl;
                    continue;
                }
                if (minDistance>=distanceToTrainingObject){
                    minDistance=distanceToTrainingObject;
                    currentNearestNeighbor=trainingObject;
                    nearestNeighborsForTestObjectId=z;
                    std::cout<<"Found nearest neighbor: "<<currentNearestNeighbor.getClassName()<<std::endl;
                }
            }
            nearestNeighborsForTestObject.push_back(currentNearestNeighbor);
            if (nearestNeighborsForTestObjectId!=10000)
                nearestNeighborsForTestObjectIds.push_back(nearestNeighborsForTestObjectId);
        }
        int nearestNeighborsFromClassAForTestObjectCount=0;
        int nearestNeighborsFromClassBForTestObjectCount=0;
        for (Object neighborForTestObject: nearestNeighborsForTestObject){
            if (neighborForTestObject.getClassNameMutable().compare("Acer")==0){
                nearestNeighborsFromClassAForTestObjectCount++;
                std::cout<<"Found acer object: "<<neighborForTestObject.getClassName()<<std::endl;
            }
            else if (neighborForTestObject.getClassNameMutable().compare("Quercus")==0){
                nearestNeighborsFromClassBForTestObjectCount++;
                std::cout<<"Found quercus object: "<<neighborForTestObject.getClassName()<<std::endl;
            }
        }
        if (nearestNeighborsFromClassAForTestObjectCount>nearestNeighborsFromClassBForTestObjectCount)
            nearestNeighborsClassNamesForTestObjects.push_back("Acer");
        else if (nearestNeighborsFromClassAForTestObjectCount<nearestNeighborsFromClassBForTestObjectCount)
            nearestNeighborsClassNamesForTestObjects.push_back("Quercus");
        nearestNeighborsForTestObject.clear();

    }
    int correctObjectsCount=0;
    for (unsigned int i=0;i<nearestNeighborsClassNamesForTestObjects.size();i++){
        if (testObjects.at(i).getClassNameMutable().compare(nearestNeighborsClassNamesForTestObjects.at(i))==0)
            correctObjectsCount++;
    }
    probability=(double)correctObjectsCount/(double)noTestObjects;
    return probability*100;
}

double Database::classifyNM()
{
    nearestMeansClassNamesForTestObjects.clear();
    double probability=0.0;
    if (k==1){
        Object classAMean;
        Object classBMean;
        classAMean.setClassName("Acer");
        classBMean.setClassName("Quercus");
        for (int i=0; i<noFeatures;i++){
            classAMean.addFeature(0.0f);
            classBMean.addFeature(0.0f);
        }
        for (int i=0; i<noFeatures;i++){
            int classAElementsCount=0, classBElementsCount=0;
            for (Object trainingObject : trainingObjects){
                if (trainingObject.compareName("Acer")==0){
                    classAMean.setFeature(i,classAMean.getFeature(i)+trainingObject.getFeature(i));
                    classAElementsCount++;
                } else if (trainingObject.compareName("Quercus")==0){
                    classBMean.setFeature(i,classBMean.getFeature(i)+trainingObject.getFeature(i));
                    classBElementsCount++;
                }
                classAMean.setFeature(i,classAMean.getFeature(i)/classAElementsCount);
                classBMean.setFeature(i,classBMean.getFeature(i)/classBElementsCount);
            }
        }
        for (Object testObject : testObjects){
            float distanceToClassA,distanceToClassB;
            for (int i=0; i<noFeatures;i++){
               distanceToClassA+=(testObject.getFeature(i)-classAMean.getFeature(i))*(testObject.getFeature(i)-classAMean.getFeature(i));
               distanceToClassB+=(testObject.getFeature(i)-classBMean.getFeature(i))*(testObject.getFeature(i)-classBMean.getFeature(i));
            }
            distanceToClassA=sqrt(distanceToClassA);
            distanceToClassB=sqrt(distanceToClassB);
//            std::vector<Object>nearestMeansForTestObject;
            if (distanceToClassA<distanceToClassB){
//                nearestMeansForTestObject.push_back(classAMean);
                nearestMeansClassNamesForTestObjects.push_back(classAMean.getClassName());
            }
            else {
//                nearestMeansForTestObject.push_back(classBMean);
                nearestMeansClassNamesForTestObjects.push_back(classBMean.getClassName());
            }
        }
        int correctObjectsCount=0;
        for (int i=0;i<nearestMeansClassNamesForTestObjects.size();i++){
            if (testObjects.at(i).compareName(nearestMeansClassNamesForTestObjects.at(i)))
                correctObjectsCount++;
        }
        probability=(double)correctObjectsCount/(double)noTestObjects;
    }
    return probability*100;
}



void Database::clear()
{
    objects.clear();
    classNamesVector.clear();
	classCounters.clear();
    featuresIDs.clear();
    noClass = 0;
    noObjects = 0;
    noFeatures = 0;
}

const std::vector<Object> &Database::getObjects() const
{
    return objects;
}

const std::map<std::__cxx11::string, int> &Database::getClassCounters() const { return classCounters; }

const std::vector<std::__cxx11::string> &Database::getClassNames() const { return classNamesVector; }

const std::vector<Object> &Database::getTrainingObjects() const
{
    return trainingObjects;
}

const std::vector<Object> &Database::getTestObjects() const
{
    return testObjects;
}

Object Database::getTrainingObjectByIndex(unsigned int index)
{
    return trainingObjects.at(index);
}

Object Database::getTestObjectByIndex(unsigned int index)
{
    return testObjects.at(index);
}

unsigned int Database::getNoClass()
{
    return classNamesVector.size();
}

unsigned int Database::getNoObjects()
{
    return noObjects;
}

unsigned int  Database::getNoFeatures()
{
	return noFeatures;
}






