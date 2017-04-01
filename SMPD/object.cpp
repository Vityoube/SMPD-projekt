#include "object.h"

void Object::setClassName(const std::string &value)
{
    className = value;
}

void Object::setFeature(int index, float featureValue)
{
    features.at(index)=featureValue;
}

void Object::addFeature(float featureValue)
{
    features.push_back(featureValue);
}

bool Object::compareName(std::__cxx11::string name)
{
    if (className.compare(name)==0)
        return true;
    return false;
}

std::string Object::getClassName() const
{
    return className;
}

    size_t Object::getFeaturesNumber() const
    {
        return features.size();
    }

    float Object::getFeature(int index) const
    {
        return features.at(index);
    }

    std::__cxx11::string Object::getClassNameMutable()
    {
        return className;
    }

    std::vector<float> Object::getFeaturesMutable()
    {
        return features;
    }

    const std::vector<float> &Object::getFeatures() const
    {
        return features;
    }

    bool Object::operator==(const Object& otherObject) const
    {        
        for (int i=0;i<this->getFeaturesNumber();i++)
            if (this->getFeature(i)!=otherObject.getFeature(i)){
                return false;
            }
        if (this->classID==otherObject.classID)
            return true;
    }



