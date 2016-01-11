#ifndef RAY_H
#define RAY_H

#include "Vector3f.h"

#include <cassert>
#include <iostream>

// Ray class mostly copied from Peter Shirley and Keith Morley
class Ray
{
  public:
	//redeclare ray class 

    Ray()
    {
	_origin=Vector3f(0.0,0.0,0.0);
	_direction=Vector3f(0.0,0.0,0.0); 
    }

    Ray(const Vector3f &orig, const Vector3f &dir) 
    {
	_direction=dir.normalized(); //we should really always normalize this, no reason we wouldnt
	_origin=orig; 
    }

    const Vector3f & getOrigin() const {
        return _origin;
    }

    const Vector3f & getDirection() const {
        return _direction;
    }

    Vector3f pointAtParameter(float t) const {
        return _origin + _direction * t;
    }

  private:
    Vector3f _origin;
    Vector3f _direction;

};

inline std::ostream &
operator<<(std::ostream &os, const Ray &r)
{
    os << "Ray <" << r.getOrigin() << ", " << r.getDirection() << ">";
    return os;
}

#endif // RAY_H
