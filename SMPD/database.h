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

    std::vector<Object> classA;
    std::vector<Object> classB;

    std::vector<std::map<std::string, std::vector<Object>>> classesGroups;

    int classACount;
    int classBCount;

    int classAObjectsCount;
    int classBObjectsCount;

    std::string currentTrainingMethod;

    std::vector<std::vector<Object>> trainingGroups;
    std::vector<std::vector<Object>> testGroups;

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
    bool loadImage(const std::string &imageName);
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
    std::vector<std::vector<Object> > getTestGroups() const;
    void setTestGroups(const std::vector<std::vector<Object> > &value);
    std::vector<std::vector<Object> > getTrainingGroups() const;
    void setTrainingGroups(const std::vector<std::vector<Object> > &value);
    std::string getCurrentTrainingMethod() const;
    void setCurrentTrainingMethod(const std::string &value);
    int getClassACount() const;
    void setClassACount(int value);
    int getClassBCount() const;
    void setClassBCount(int value);
    std::vector<Object> getClassA() const;
    void setClassA(const std::vector<Object> &value);
    std::vector<Object> getClassB() const;
    void setClassB(const std::vector<Object> &value);

    std::vector<Object> getTestGroup(int index);
    std::vector<Object> getTrainingGroup(int index);
    std::vector<std::map<std::string, std::vector<Object> > > getClassesGroups() const;
    void setClassesGroups(const std::vector<std::map<std::string, std::vector<Object> > > &value);
};


#endif // DATABASE_H
