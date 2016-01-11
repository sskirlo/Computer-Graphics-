#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"

#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// Choose your representation, add more fields and fill in the functions
class Plane: public Object3D
{
  public:
    Plane(const Vector3f &normal, float d, Material *m) :
        Object3D(m)
    {
	_normal=normal; //we normalized here, maybe shouldn't have 
	_d=-d; 
    }

    virtual bool intersect(const Ray &r, float tmin, Hit &h) const 
    {
	//return false; 
	
	//Vector3f D=(r.getDirection()).normalized();
	Vector3f D=(r.getDirection()); //need to be careful because applying transformation requires that this not be normalized, to be consistent
	Vector3f O=r.getOrigin();

	float t=-(_d+O.dot(O,_normal))/(D.dot(D,_normal));

	if( (t>tmin) && (t<h.getT()) ) //also have to check that in front of image plane 
	{
		h.set(t,getMaterial(),_normal); 
		h.setTexFalse();
		return true;
	}
	else
	{
        	return false;
	}  
    }

 private:
	Vector3f _normal;
	float _d; 
};
#endif //PLANE_H


