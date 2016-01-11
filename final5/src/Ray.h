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
	//_direction=dir.normalized(); //we should really always normalize this, no reason we wouldnt
		_direction=dir; //big mistake, need to make sure not to normalize direction, this screws things up, this is why fixing transform 
	    _origin=orig;   //class didn't help! 
    }

    const Vector3f & getOrigin() const {
        return _origin;
    }

	//get rid of &, we are really referencing the address, not the actual object so we do not have to make a new copy 

    const Vector3f getDirection() const {
        //return (_direction.normalized()); //lets try this out of desparation
		return  (_direction); //lets try this out of desparation, this won't help other intersection methods need this 
    }

    Vector3f pointAtParameter(float t) const {
        //return _origin + (_direction.normalized()) * t;  //this is crazy lets just try it
        return _origin + _direction * t;  //need to preserve normalization of direction if we want to identify point properly 
                                          //t will be the same in different frames 
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
