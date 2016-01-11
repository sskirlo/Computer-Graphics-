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
	Vector3f d=r.getDirection(); //didn't normalize here, good  
	o=o-center; //shift origin 
	float a=d.dot(d,d); //before we assumed d was normalized, which was bad 
	float c=o.dot(o,o)-pow(radius,2.0); 
	float b=2*o.dot(o,d); 
	float det=pow(b,2.0)-4*c*a; 

	//cout << "radius " << radius << endl; 
	//cout << o[0] << " " << o[1] << " " << o[2] <<endl; 
	//cout << d[0] << " " << d[1] << " " << d[2] <<endl; //check if ray normalized 

	if(det<0)
	{
		//cout << "doesn't hit sphere " <<endl; 
	    return false;  //no solution
	}
	else
	{
		//cout << "Hits sphere " <<endl; 
		
		float t1=(-b+sqrt(det))/(2.0*a); 
		float t2=(-b-sqrt(det))/(2.0*a);
		
		//cout << "tmin t1 t2 h.getT " << tmin << " " << t1 << " " << t2 << " " << h.getT() <<endl; 

		//need to handle logic carefully here for case of casting ray from sphere surface, if one of the solutions 
		//is within tmin, we need to discard it, and compare with the next smallest solution, we 
		//didn't handle this case properly, also would be a problem if we are casting from anywhere inside a sphere 
		
		float tcomp=fmin(t1,t2); 
		if(tcomp<tmin)  //throw away lower solution if below tmin, and only compare to upper solution for logic of program
		{
			tcomp=fmax(t1,t2); 
		}
		
		if( (tmin<tcomp) && (h.getT()>tcomp) )
		{
			//cout << "hit sphere t1, t2 tmin " << t1 << " "<< t2 << " "<< tmin <<endl; 

			//calculate normal
			Vector3f normal=(r.pointAtParameter(tcomp)-center);
			h.set(tcomp,getMaterial(),normal.normalized());
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
