#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <algorithm>

#include "object.h"

class Database
{
private:
    std::vector<Object> objects;
    std::vector<Object> trainingObjects;
    std::vector<Object> testObjects;
    std::map<std::string, int> classCounters;
	std::vector<std::string> classNamesVector;
    std::vector<unsigned int> featuresIDs;
    std::vector<unsigned int> trainObjectsIds;

    unsigned int noClass;
    unsigned int noObjects;
    unsigned int noFeatures;
    unsigned int noTrainingObjects;
    unsigned int noTestObjects;
    unsigned int k;



public:

    Database();

    bool addObject(const Object &object);
    void clear();
    bool load(const std::string &fileName);
    void save(const std::string &fileName);
    bool trainObjects(double trainingPartPercent);
    double classifyNN(int k);
    double classifyNM(int k);

    const std::vector<Object> &getObjects() const;
    const std::map<std::string, int>& getClassCounters() const;
    const std::vector<std::string>& getClassNames() const;
    const std::vector<Object> &getTrainingObjects() const;
    const std::vector<Object> &getTestObjects() const;

    Object getTrainingObjectByIndex(unsigned int index);
    Object getTestObjectByIndex(unsigned int index);

    unsigned int getNoClass();
    unsigned int getNoObjects();
    unsigned int getNoFeatures();

    unsigned int getNoTrainingObjects() const;
    unsigned int getNoTestObjects() const;

    void clearObjects();
    unsigned int getK() const;
    void setK(unsigned int value);
};


#endif // DATABASE_H
