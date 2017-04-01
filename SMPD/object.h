#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>

class Object
{
private:
	int classID;
    std::string className;
    std::vector<float> features;


public:

    Object(const std::string &className, const std::vector<float> &features) :classID(-1), className(className), features(features)
    {
    }
    Object()
    {

    }

    std::string getClassName() const;
    size_t getFeaturesNumber() const;
    float getFeature(int index);
    std::string getClassNameMutable();
    std::vector<float> getFeaturesMutable();
    const std::vector<float> &getFeatures() const;
    bool operator == (const Object& otherObject) const;
    void setClassName(const std::string &value);
    void setFeature(int index, float featureValue);
    void addFeature(float featureValue);
    bool compareName(std::string name);

};



#endif // OBJECT_H
