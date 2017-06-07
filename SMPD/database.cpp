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

std::vector<Object> Database::getClassAObjects() const
{
    return classAObjects;
}

void Database::setClassAObjects(const std::vector<Object> &value)
{
    classAObjects = value;
}

std::vector<Object> Database::getClassBObjects() const
{
    return classBObjects;
}

void Database::setClassBObjects(const std::vector<Object> &value)
{
    classBObjects = value;
}

int Database::getClassAObjectsCount() const
{
    return classAObjectsCount;
}

void Database::setClassAObjectsCount(int value)
{
    classAObjectsCount = value;
}

int Database::getClassBObjectsCount() const
{
    return classBObjectsCount;
}

Object Database::getObjectByIndex(int index)
{
    return objects.at(index);
}

void Database::setClassBObjectsCount(int value)
{
    classBObjectsCount = value;
}

std::vector<std::vector<Object> > Database::getTestGroups() const
{
    return testGroups;
}

void Database::setTestGroups(const std::vector<std::vector<Object> > &value)
{
    testGroups = value;
}

std::vector<std::vector<Object> > Database::getTrainingGroups() const
{
    return trainingGroups;
}

void Database::setTrainingGroups(const std::vector<std::vector<Object> > &value)
{
    trainingGroups = value;
}

std::string Database::getCurrentTrainingMethod() const
{
    return currentTrainingMethod;
}

void Database::setCurrentTrainingMethod(const std::string &value)
{
    currentTrainingMethod = value;
}

int Database::getClassACount() const
{
    return classACount;
}

void Database::setClassACount(int value)
{
    classACount = value;
}

int Database::getClassBCount() const
{
    return classBCount;
}

void Database::setClassBCount(int value)
{
    classBCount = value;
}

std::vector<Object> Database::getClassA() const
{
    return classA;
}

void Database::setClassA(const std::vector<Object> &value)
{
    classA = value;
}

std::vector<Object> Database::getClassB() const
{
    return classB;
}

void Database::setClassB(const std::vector<Object> &value)
{
    classB = value;
}

std::vector<Object> Database::getTestGroup(int index)
{
    return testGroups.at(index);
}

std::vector<Object> Database::getTrainingGroup(int index)
{
    return trainingGroups.at(index);
}

std::vector<std::map<std::string, std::vector<Object> > > Database::getClassesGroups() const
{
    return classesGroups;
}

void Database::setClassesGroups(const std::vector<std::map<std::string, std::vector<Object> > > &value)
{
    classesGroups = value;
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
    if (object.getClassName().compare("Acer")==0)
        classA.push_back(object);
    else if (object.getClassName().compare("Quercus")==0)
        classB.push_back(object);
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
//                std::cout<<"Feature value string: "<<feature<<std::endl;
                features = features.substr(pos + 1);
                float featureValue = std::stof(feature);
                featuresValues.push_back(featureValue);
//                std::cout<<"Feature value: "<<std::fixed<<std::setprecision(5)<<featureValue<<std::endl;
            }
            else
            {
                float featureValue = std::stof(features);
//                std::cout<<"Feature value string: "<<features<<std::endl;
                featuresValues.push_back(featureValue);
//                std::cout<<"Feature value: "<<std::fixed<<std::setprecision(5)<<featureValue<<std::endl;
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
    classACount=classA.size();
    classBCount=classB.size();
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

bool Database::trainObjects(double trainingPartPercent,std::string method)
{
//    testObjects.clear();
//    trainingObjects.clear();
//    trainObjectsIds.clear();
    classesGroups.clear();
    testGroups.clear();
    trainingGroups.clear();
    currentTrainingMethod=method;
    if (currentTrainingMethod.compare("Standart")==0){
        if (ceil(objects.size()*trainingPartPercent)<objects.size()){
            std::vector<Object> trainingObjects;
            std::vector<Object> testObjects;
            std::vector<int> trainObjectsIds;
            int noTrainingObjects=objects.size()*trainingPartPercent;
            while(trainingObjects.size()<noTrainingObjects){
                unsigned int currentObjectId=rand()%noObjects;
                if (std::find(trainObjectsIds.begin(),trainObjectsIds.end(),currentObjectId)!=trainObjectsIds.end())
                    continue;
                trainingObjects.push_back(objects.at(currentObjectId));
                trainObjectsIds.push_back(currentObjectId);
            }
            int noTestObjects=objects.size()-noTrainingObjects;
            std::cout<<"objects size: "<<objects.size()<<std::endl;
            for (int i=0;i<objects.size();i++){
                Object object=objects.at(i);
                std::cout<<"current object: "<<object.getClassName()<<std::endl;
                if (std::find(trainObjectsIds.begin(),trainObjectsIds.end(),i)!=trainObjectsIds.end())
                    continue;
                testObjects.push_back(object);
                std::cout<<"testObjects size: "<<testObjects.size()<<std::endl;
            }
            trainingGroups.push_back(trainingObjects);
            testGroups.push_back(testObjects);

        } else {
            return false;
        }
    } else if (method.compare("Bootstrap")==0){
        int trainingPart=(int)(trainingPartPercent*100);
        if(trainingPart<noObjects && trainingPart>0){            
            for (int currentGroup=0;currentGroup<trainingPart;currentGroup++){
                std::vector<int> bootstrapObjectsIds;
                std::vector<Object> currentTrainingGroup;
                std::vector<Object> currentTestGroup;
                std::vector<int> currentTrainingObjectsIds;
                for (int i=0;i<trainingPart;i++){
                    int currentIndex=rand()%noObjects;
                    bootstrapObjectsIds.push_back(currentIndex);                 \
                }
                for (int bootstrapObjectId : bootstrapObjectsIds){
                    if (std::find(currentTrainingObjectsIds.begin(),currentTrainingObjectsIds.end(),bootstrapObjectId)==currentTrainingObjectsIds.end()){
                        Object bootstrapObject=objects.at(bootstrapObjectId);
                        currentTrainingGroup.push_back(bootstrapObject);
                        currentTrainingObjectsIds.push_back(bootstrapObjectId);
                    }
                }
                for (int i=0;i<noObjects;i++){
                    if (std::find(currentTrainingObjectsIds.begin(),currentTrainingObjectsIds.end(),i)==currentTrainingObjectsIds.end()){
                        Object testObject=objects.at(i);
                        currentTestGroup.push_back(testObject);
                    }
                }
                trainingGroups.push_back(currentTrainingGroup);
                testGroups.push_back(currentTestGroup);
                std::cout<<"Training groups count: "<<trainingGroups.size()<<std::endl;
                std::cout<<"Test groups count: "<<testGroups.size()<<std::endl;
            }
        } else {
            return false;
        }
    } else if (method.compare("Cross validation")){
        int trainingPartsCount=(int)(trainingPartPercent*100);
        std::cout<<"Parts count: "<<trainingPartsCount<<std::endl;
        std::vector<std::vector<Object>> objectsGroups;
        if (trainingPartsCount<noObjects && trainingPartsCount>0){
            int lastPartBegin=0;
            for (int currentPartBegin=0;currentPartBegin<noObjects-(noObjects/(trainingPartsCount-1));
                 currentPartBegin+=(int)(noObjects/trainingPartsCount)){
//                std::cout<<"Current part begin index: "<<currentPartBegin<<std::endl;
                int currentPartEnd=currentPartBegin+(int)(noObjects/trainingPartsCount);
                if (currentPartEnd>noObjects)
                    currentPartEnd=noObjects;
                std::vector<Object> currentGroup;
                for (int i=currentPartBegin;i<currentPartEnd;i++){
                    currentGroup.push_back(objects.at(i));
                }
//                std::cout<<"Current group size: "<<currentGroup.size()<<std::endl;
//                std::cout<<"Current end index: "<<currentPartEnd<<std::endl;
                objectsGroups.push_back(currentGroup);
                lastPartBegin=currentPartEnd;
//                std::cout<<"current Group Size: "<<currentGroup.size()<<std::endl;
//                std::cout<<"groups count: "<<objectsGroups.size()<<std::endl;
            }
            std::vector<Object> lastPartGroup;
            for (int i=lastPartBegin;i<noObjects;i++){
                lastPartGroup.push_back(objects.at(i));
            }
            objectsGroups.push_back(lastPartGroup);
            std::vector<Object> trainingGroup;
            for (int i=0;i<objectsGroups.size();i++){
                std::vector<Object> currentGroup=objectsGroups.at(i);
//                std::cout<<"Current test group size: "<<currentGroup.size()<<std::endl;
                testGroups.push_back(currentGroup);
                for (int j=0;j<objectsGroups.size();j++){
                    if (j!=i){
                        std::vector<Object> currentTrainingGroup=objectsGroups.at(j);
                        for (Object currentTrainingObject : currentTrainingGroup){
                            trainingGroup.push_back(currentTrainingObject);
                        }
                    }
                }
//                std::cout<<"Current training group size: "<<trainingGroup.size()<<std::endl;
                trainingGroups.push_back(trainingGroup);
                trainingGroup.clear();
//                std::cout<<"Training groups count: "<<trainingGroups.size()<<std::endl;
//                std::cout<<"Test groups count: "<<testGroups.size()<<std::endl;
            }
        } else {
            return false;
        }
    }

    for (std::vector<Object> currentTrainingGroup : trainingGroups){
        std::map<std::string,std::vector<Object>> currentClassesGroup;
        std::vector<Object> classA, classB;
        for (Object currentTrainingObject : currentTrainingGroup){
            if (currentTrainingObject.compareName("Acer"))
                classA.push_back(currentTrainingObject);
            else if (currentTrainingObject.compareName("Quercus"))
               classB.push_back(currentTrainingObject);
        }
        currentClassesGroup.insert(std::pair<std::string,std::vector<Object>>("A",classA));
        currentClassesGroup.insert(std::pair<std::string,std::vector<Object>>("B",classB));
        classesGroups.push_back(currentClassesGroup);
    }
    return true;

}

double Database::classifyNN()
{
    std::vector<int> nearestNeighborsForTestObjectIds;
    double finalProbability=0.0;
    std::vector<double> probabilities;
    std::cout<<"Before main loop. trainingObjects size: "<<trainingObjects.size()<<std::endl;
    for (int currentGroup=0;currentGroup<testGroups.size();currentGroup++){
        std::vector<Object> currentTestObjects=testGroups.at(currentGroup);
        std::vector<Object> currentTrainingObjects=trainingGroups.at(currentGroup);
        nearestNeighborsClassNamesForTestObjects.clear();
        double probability=0.0;
        for (Object testObject : currentTestObjects){
            std::cout<<"In main loop"<<std::endl;
            float minDistance=99999.9f;
            for (int i=0;i<k;i++){
                int nearestNeighborsForTestObjectId=10000;
                for (int z=0;z<currentTrainingObjects.size();z++){
                    Object trainingObject=currentTrainingObjects.at(z);
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
            if (currentTestObjects.at(i).getClassNameMutable().compare(nearestNeighborsClassNamesForTestObjects.at(i))==0)
                correctObjectsCount++;
        }
        probability=(double)correctObjectsCount/(double)currentTestObjects.size();
        std::cout<<"Current propability: "<<probability<<std::endl;
        probabilities.push_back(probability);
    }
    for (double probability : probabilities){
        finalProbability+=probability;
    }
    finalProbability/=(double)probabilities.size();
    return finalProbability*100;
}

double Database::classifyNM()
{
    double finalProbability=0.0;
    if (k==1){
        std::vector<double> probabilities;
        for (int currentGroup=0;currentGroup<testGroups.size();currentGroup++){
            nearestMeansClassNamesForTestObjects.clear();
            std::vector<Object> currentTestObjects=testGroups.at(currentGroup);
            std::vector<Object> currentTrainingObjects=trainingGroups.at(currentGroup);
            double probability=0.0;
            Object classAMean;
            Object classBMean;
            classAMean.setClassName("Acer");
            classBMean.setClassName("Quercus");
            for (int i=0; i<noFeatures;i++){
                classAMean.addFeature(0.0f);
                classBMean.addFeature(0.0f);
                int classAElementsCount=0, classBElementsCount=0;
                for (Object trainingObject : currentTrainingObjects){
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
            std::cout<<"Calculating distance from test objects"<<std::endl;
            for (Object testObject : currentTestObjects){
                float distanceToClassA=0.0,distanceToClassB=0.0;
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
                if (currentTestObjects.at(i).compareName(nearestMeansClassNamesForTestObjects.at(i)))
                    correctObjectsCount++;
            }
            probability=(double)correctObjectsCount/(double)currentTestObjects.size();
            probabilities.push_back(probability);

        }
        for (double probability : probabilities){
            finalProbability+=probability;
        }
        finalProbability/=(double)probabilities.size();
    } else {
        std::vector<double> probabilities;
        for (int currentGroup=0;currentGroup<testGroups.size();currentGroup++){
            double probability=0.0;
            meansForAClassGroups.clear();
            meansForBClassGroups.clear();
            groupsForAClass.clear();
            groupsForBClass.clear();
            nearestMeansClassNamesForTestObjects.clear();
            std::map<std::string,std::vector<Object>> classes=classesGroups.at(currentGroup);
            std::vector<Object> classAObjects=classes.at("A");
            std::vector<Object> classBObjects=classes.at("B");
            std::vector<Object> currentTestObjects=testGroups.at(currentGroup);
            int classAObjectsCount=classAObjects.size(), classBObjectsCount=classBObjects.size();
            std::vector<int> currentChoicesIndexesForA;
            for (int i=0;i<k;i++){
                int currentChoiceIndex=rand()%classAObjects.size();
                if (std::find(currentChoicesIndexesForA.begin(),currentChoicesIndexesForA.end(),currentChoiceIndex)==currentChoicesIndexesForA.end()){
                    Object currentMean=classAObjects.at(currentChoiceIndex);
                    meansForAClassGroups.push_back(currentMean);
                    currentChoicesIndexesForA.push_back(currentChoiceIndex);
                }
            }
            std::vector<int> currentChoicesIndexesForB;
            for (int i=0;i<k;i++){
                int currentChoiceIndex=rand()%classBObjects.size();
                if (std::find(currentChoicesIndexesForB.begin(),currentChoicesIndexesForB.end(),currentChoiceIndex)==currentChoicesIndexesForB.end()){
                    Object currentMean=classBObjects.at(currentChoiceIndex);
                    meansForBClassGroups.push_back(currentMean);
                    currentChoicesIndexesForB.push_back(currentChoiceIndex);
                }
            }
            for (int i=0;i<k;i++){
                groupsForAClass.insert(std::pair<int,std::vector<Object>>(i,std::vector<Object>()));
                groupsForBClass.insert(std::pair<int,std::vector<Object>>(i,std::vector<Object>()));
            }

            int meansCalculateCount=10;
            for(int i=0; i<classAObjectsCount;i++){
                if (std::find(currentChoicesIndexesForA.begin(),currentChoicesIndexesForA.end(),i)!=currentChoicesIndexesForA.end())
                    continue;
                float minDistanceFromClassAObjectToMean=99999999999.9f;
                Object currentClassAObject=classAObjects.at(i);
                int groupForCurrentClassObject=0;
                for (int j=0; j<meansForAClassGroups.size();j++){
                    Object currentMean=meansForAClassGroups.at(j);
                    float distanceFromObjectAToMean=0.0f;
                    for (int featureNo=0; featureNo<noFeatures;featureNo++){
                        distanceFromObjectAToMean+=(currentClassAObject.getFeature(featureNo)-currentMean.getFeature(featureNo))*
                                (currentClassAObject.getFeature(featureNo)-currentMean.getFeature(featureNo));
                    }
                    distanceFromObjectAToMean=(float)sqrt((double)distanceFromObjectAToMean)/(float)noFeatures;
                    if (distanceFromObjectAToMean<=minDistanceFromClassAObjectToMean){
                        minDistanceFromClassAObjectToMean=distanceFromObjectAToMean;
                        groupForCurrentClassObject=j;
                    }
                }
                std::vector<Object> nearestGroupForClassAObject=groupsForAClass[groupForCurrentClassObject];
                nearestGroupForClassAObject.push_back(currentClassAObject);
                groupsForAClass[groupForCurrentClassObject]=nearestGroupForClassAObject;
            }
            for(int i=0; i<classBObjectsCount;i++){
                if (std::find(currentChoicesIndexesForB.begin(),currentChoicesIndexesForB.end(),i)!=currentChoicesIndexesForB.end())
                    continue;
                float minDistanceFromClassBObjectToMean=99999999999.9f;
                Object currentClassBObject=classBObjects.at(i);
                int groupForCurrentClassObject=0;
                for (int j=0; j<meansForBClassGroups.size();j++){
                    Object currentMean=meansForBClassGroups.at(j);
                    float distanceFromObjectBToMean=0.0f;
                    for (int featureNo=0; featureNo<noFeatures;featureNo++){
                        distanceFromObjectBToMean+=(currentClassBObject.getFeature(featureNo)-currentMean.getFeature(featureNo))*
                                (currentClassBObject.getFeature(featureNo)-currentMean.getFeature(featureNo));
                    }
                    distanceFromObjectBToMean=(float)sqrt((double)distanceFromObjectBToMean)/(float)noFeatures;
                    if (distanceFromObjectBToMean<=minDistanceFromClassBObjectToMean){
                        minDistanceFromClassBObjectToMean=distanceFromObjectBToMean;
                        groupForCurrentClassObject=j;
                    }
                }
                std::vector<Object> nearestGroupForClassBObject=groupsForBClass[groupForCurrentClassObject];
                nearestGroupForClassBObject.push_back(currentClassBObject);
                groupsForBClass[groupForCurrentClassObject]=nearestGroupForClassBObject;
            }
            for (int currentGroupIndex=0;currentGroupIndex<meansForAClassGroups.size();currentGroupIndex++){
                Object currentMean=meansForAClassGroups.at(currentGroupIndex);
                for (int currentFeature=0; currentFeature<noFeatures;currentFeature++){
                        std::vector<Object> elementsFromClassAGroup=groupsForAClass.at(currentGroupIndex);
                        float currentFeatureValue=0.0f;
                        for (Object currentObjectFromGroup : elementsFromClassAGroup)
                            currentFeatureValue+=currentObjectFromGroup.getFeature(currentFeature);
                        currentFeatureValue/=elementsFromClassAGroup.size();
                        currentMean.setFeature(currentFeature,currentFeatureValue);
                    }
                }
            for (int currentGroupIndex=0;currentGroupIndex<meansForBClassGroups.size();currentGroupIndex++){
                Object currentMean=meansForBClassGroups.at(currentGroupIndex);
                for (int currentFeature=0; currentFeature<noFeatures;currentFeature++){
                        std::vector<Object> elementsFromClassBGroup=groupsForBClass.at(currentGroupIndex);
                        float currentFeatureValue=0.0f;
                        for (Object currentObjectFromGroup : elementsFromClassBGroup)
                            currentFeatureValue+=currentObjectFromGroup.getFeature(currentFeature);
                        currentFeatureValue/=elementsFromClassBGroup.size();
                        currentMean.setFeature(currentFeature,currentFeatureValue);
                }
             }
            for (int currentStep=1;currentStep<meansCalculateCount;currentStep++){


                for(Object currentClassAObject : classAObjects){
                    float minDistanceFromClassAObjectToMean=99999999999.9f;
                    int groupForCurrentClassObject=0;
                    for (int j=0; j<meansForAClassGroups.size();j++){
                        Object currentMean=meansForAClassGroups.at(j);
                        float distanceFromObjectAToMean=0.0f;
                        for (int featureNo=0; featureNo<noFeatures;featureNo++){
                            distanceFromObjectAToMean+=(currentClassAObject.getFeature(featureNo)-currentMean.getFeature(featureNo))*
                                    (currentClassAObject.getFeature(featureNo)-currentMean.getFeature(featureNo));
                        }
                        distanceFromObjectAToMean=(float)sqrt((double)distanceFromObjectAToMean)/(float)noFeatures;
                        if (distanceFromObjectAToMean<=minDistanceFromClassAObjectToMean){
                            minDistanceFromClassAObjectToMean=distanceFromObjectAToMean;
                            groupForCurrentClassObject=j;
                        }
                    }
                    std::vector<Object>nearestGroupForClassAObject=groupsForAClass[groupForCurrentClassObject];
                    nearestGroupForClassAObject.push_back(currentClassAObject);
                    groupsForAClass[groupForCurrentClassObject]=nearestGroupForClassAObject;
                }
                for(Object currentClassBObject : classBObjects){
                    float minDistanceFromClassBObjectToMean=99999999999.9f;
                    int groupForCurrentClassObject=0;
                    for (int j=0; j<meansForBClassGroups.size();j++){
                        Object currentMean=meansForBClassGroups.at(j);
                        float distanceFromObjectBToMean=0.0f;
                        for (int featureNo=0; featureNo<noFeatures;featureNo++){
                            distanceFromObjectBToMean+=(currentClassBObject.getFeature(featureNo)-currentMean.getFeature(featureNo))*
                                    (currentClassBObject.getFeature(featureNo)-currentMean.getFeature(featureNo));
                        }
                        distanceFromObjectBToMean=(float)sqrt((double)distanceFromObjectBToMean)/(float)noFeatures;
                        if (distanceFromObjectBToMean<=minDistanceFromClassBObjectToMean){
                            minDistanceFromClassBObjectToMean=distanceFromObjectBToMean;
                            groupForCurrentClassObject=j;
                        }
                    }
                    std::vector<Object> nearestGroupForClassBObject=groupsForBClass[groupForCurrentClassObject];
                    nearestGroupForClassBObject.push_back(currentClassBObject);
                    groupsForBClass[groupForCurrentClassObject]=nearestGroupForClassBObject;
                }
                for (int currentGroupIndex=0;currentGroupIndex<meansForAClassGroups.size();currentGroupIndex++){
                    Object currentMean=meansForAClassGroups.at(currentGroupIndex);
                    for (int currentFeature=0; currentFeature<noFeatures;currentFeature++){
                            std::vector<Object> elementsFromClassAGroup=groupsForAClass.at(currentGroupIndex);
                            float currentFeatureValue=0.0f;
                            for (Object currentObjectFromGroup : elementsFromClassAGroup)
                                currentFeatureValue+=currentObjectFromGroup.getFeature(currentFeature);
                            currentFeatureValue/=elementsFromClassAGroup.size();
                            currentMean.setFeature(currentFeature,currentFeatureValue);
                        }
                    }
                for (int currentGroupIndex=0;currentGroupIndex<meansForBClassGroups.size();currentGroupIndex++){
                    Object currentMean=meansForBClassGroups.at(currentGroupIndex);
                    for (int currentFeature=0; currentFeature<noFeatures;currentFeature++){
                            std::vector<Object> elementsFromClassBGroup=groupsForBClass.at(currentGroupIndex);
                            float currentFeatureValue=0.0f;
                            for (Object currentObjectFromGroup : elementsFromClassBGroup)
                                currentFeatureValue+=currentObjectFromGroup.getFeature(currentFeature);
                            currentFeatureValue/=elementsFromClassBGroup.size();
                            currentMean.setFeature(currentFeature,currentFeatureValue);
                    }
                 }

            }

            std::vector<Object> meansForAllClasses;
            meansForAllClasses.insert(meansForAllClasses.end(),meansForAClassGroups.begin(),meansForAClassGroups.end());
            meansForAllClasses.insert(meansForAllClasses.end(),meansForBClassGroups.begin(),meansForBClassGroups.end());
            for (Object testObject : currentTestObjects){
                int nearestMeanForTestObjectIndex=0;
                float minDistanceToMean=99999999999.9f;
                for (int currentMeansIndex=0;currentMeansIndex<meansForAllClasses.size();currentMeansIndex++){
                    Object currentMean=meansForAllClasses.at(currentMeansIndex);
                    float distanceToMean=0.0f;
                    for (int i=0; i<noFeatures;i++){
                        distanceToMean+=(testObject.getFeature(i)-currentMean.getFeature(i))*(testObject.getFeature(i)-currentMean.getFeature(i));
                    }
                    distanceToMean=(float)sqrt((double)distanceToMean)/(float)noFeatures;
                    if (distanceToMean<=minDistanceToMean){
                        minDistanceToMean=distanceToMean;
                        nearestMeanForTestObjectIndex=currentMeansIndex;
                    }
                }
                Object nearestMean = meansForAllClasses.at(nearestMeanForTestObjectIndex);
                nearestMeansClassNamesForTestObjects.push_back(nearestMean.getClassName());
    //            std::cout<<"Found nearest mean: "<<nearestMean.getClassName()<<std::endl;
            }

            std::cout<<"testObjects size: "<<currentTestObjects.size()<<std::endl<<
                    "nearestMeans size: "<<nearestMeansClassNamesForTestObjects.size()<<std::endl;
            int algorythmRightAnswers=0;
            for (int i=0;i<nearestMeansClassNamesForTestObjects.size();i++){
                Object testObject=currentTestObjects.at(i);
                std::string currentAnswer=nearestMeansClassNamesForTestObjects.at(i);
                if (testObject.compareName(currentAnswer))
                    algorythmRightAnswers++;
            }
            std::cout<<"Right answers: "<<algorythmRightAnswers<<std::endl;
            probability=(double)algorythmRightAnswers/(double)currentTestObjects.size();
            probabilities.push_back(probability);
        }
        for (double probability : probabilities){
            finalProbability+=probability;
        }
        finalProbability/=(double)probabilities.size();
    }
    return finalProbability*100;
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






