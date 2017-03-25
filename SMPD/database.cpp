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
        for (const Object object : objects){
            if (std::find(trainingObjects.begin(),trainingObjects.end(),object)!=trainingObjects.end())
                continue;
            testObjects.push_back(object);
        }
        return true;
    }
    return false;

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






