#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"

#include <vecmath.h>
#include <cmath>

using namespace std; 

// TODO:
// Implement functions and add more fields as necessary
class Sphere : public Object3D
{
  public:
    Sphere() { 
        // unit ball at the center
	radius=1; 
	center=Vector3f(0.0,0.0,0.0); 
    }

    Sphere(const Vector3f &cent, 
           float rad, 
           Material *material) :
        Object3D(material)  
    {
        // TODO: implement
	center=cent;	
	radius=rad; 
	//material private variable already taken care of by Object3D class
    }

    virtual bool intersect(const Ray &r, float tmin, Hit &h) const
    {
	Vector3f o=r.getOrigin(); 
	Vector3f d=r.getDirection(); 
	o=o-center; //shift origin 
	float c=o.dot(o,o)-pow(radius,2.0); 
	float b=2*o.dot(o,d); 
	float det=pow(b,2.0)-4*c; 

	//cout << "radius " << radius << endl; 
	//cout << o[0] << " " << o[1] << " " << o[2] <<endl; 
	//cout << d[0] << " " << d[1] << " " << d[2] <<endl; //check if ray normalized 

	if(det<0)
	{
	        return false;  //no solution
	}
	else
	{
		float t1=(-b+sqrt(det))/2.0; 
		float t2=(-b-sqrt(det))/2.0; 


		if( (tmin<fmin(t1,t2)) && (h.getT()>fmin(t1,t2)) )
		{
			cout << "hit sphere t1, t2 tmin " << t1 << " "<< t2 << " "<< tmin <<endl; 

			//calculate normal
			Vector3f normal=(r.pointAtParameter(fmin(t1,t2))-center);
			h.set(fmin(t1,t2),getMaterial(),normal.normalized());
			h.setTexFalse(); //need to set Tex state to false in case in front of an object
					//which had Tex
			return true; 
		}
		else
		{
			return false; 
		}
	}
    }

  private:
	float radius; 
	Vector3f center; 

};

#endif
