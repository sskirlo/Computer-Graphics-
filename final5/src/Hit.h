#ifndef HIT_H
#define HIT_H

#include "Ray.h"
#include "vecmath.h"

#include <iostream>
#include <limits>

class Material;

class Hit
{
  public:
    // Constructors
    Hit() :
        _material(NULL),
        _t(std::numeric_limits<float>::max()),
        _hasTex(false)
    {
    }

    Hit(float t, Material *material, const Vector3f &normal) :
        _t(t),
        _material(material),
        _normal(normal),
        _hasTex(false)
    {
    }

    // Destructor
    ~Hit()
    {
    }

    float getT() const
    {
        return _t;
    }

    Material * getMaterial() const
    {
        return _material;
    }

    const Vector3f & getNormal() const
    {
        return _normal;
    }

    void set(float t, Material *material, const Vector3f &normal)
    {
        _t = t;
        _material = material;
        _normal = normal;
    }

    void setTexCoord(const Vector2f &texCoord) {
        _texCoord = texCoord;
        _hasTex = true;
    }

    void setTexFalse()  {
        _hasTex=false;
    }

    bool hasTex() const {
        return _hasTex;
    }

    const Vector2f & getTexCoord() const {
        return _texCoord;
    }

  private:
    float _t;
    Material *_material;
    Vector3f _normal;
    bool _hasTex;
    Vector2f _texCoord;
};

inline std::ostream & 
operator<<(std::ostream &os, const Hit &h)
{
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
