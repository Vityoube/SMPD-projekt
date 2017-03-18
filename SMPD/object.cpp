#include "object.h"

    std::string Object::getClassName() const
    {
        return className;
    }

    size_t Object::getFeaturesNumber() const
    {
        return features.size();
    }

    const std::vector<float> &Object::getFeatures() const
    {
        return features;
    }

    bool Object::operator ==(const Object& otherObject) const
    {
          return (this->getFeatures()==otherObject.getFeatures());
    }



