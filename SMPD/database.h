#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <list>
#include <map>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <boost/lexical_cast.hpp>

#include "object.h"
#include "matrixutil.hpp"
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
    std::vector<Object> nearestNeighborsForTestObject;
    std::vector<std::string> nearestNeighborsClassNamesForTestObjects;
    Object currentNearestNeighbor;
    std::map<Object,std::vector<Object>> nearestMeansForTestObjects;
    std::vector<std::string> nearestMeansClassNamesForTestObjects;

    std::map<int,std::vector<Object>> groupsForAClass;
    std::map<int,std::vector<Object>> groupsForBClass;

    std::vector<Object> meansForAClassGroups;
    std::vector<Object> meansForBClassGroups;

    std::vector<Object> classAObjects;
    std::vector<Object> classBObjects;
    int classAObjectsCount;
    int classBObjectsCount;

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
    bool trainObjects(double trainingPartPercent, std::string method);
    double classifyNN();
    double classifyNM();

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
    std::map<Object, std::vector<Object> > getNearestMeansForTestObjects() const;
    void setNearestMeansForTestObjects(const std::map<Object, std::vector<Object> > &value);
    std::vector<Object> getClassAObjects() const;
    void setClassAObjects(const std::vector<Object> &value);
    std::vector<Object> getClassBObjects() const;
    void setClassBObjects(const std::vector<Object> &value);
    int getClassAObjectsCount() const;
    void setClassAObjectsCount(int value);
    int getClassBObjectsCount() const;

    Object getObjectByIndex(int index);
    void setClassBObjectsCount(int value);
};


#endif // DATABASE_H
