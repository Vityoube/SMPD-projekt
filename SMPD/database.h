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

public:

    Database() : noClass(0), noObjects(0), noFeatures(0)
    {
    }

    bool addObject(const Object &object);
    void clear();
    bool load(const std::string &fileName);
    void save(const std::string &fileName);
    bool trainObjects(double trainingPartPercent);

    const std::vector<Object> &getObjects() const;
	const std::map<std::string, int>& getClassCounters() const { return classCounters; }
	const std::vector<std::string>& getClassNames() const { return classNamesVector; }
    const std::vector<Object> &getTrainingObjects() const;
    const std::vector<Object> &getTestObjects() const;

    unsigned int getNoClass();
    unsigned int getNoObjects();
    unsigned int getNoFeatures();

    unsigned int getNoTrainingObjects() const;
    unsigned int getNoTestObjects() const;

    void clearObjects();
};


#endif // DATABASE_H
