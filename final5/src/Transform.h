#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Object3D.h"
#include "vecmath.h"

using namespace std; 

// TODO implement this class
// So that the intersect function first transforms the ray
// Add more fields as necessary
class Transform: public Object3D
{
  public: 
    Transform(const Matrix4f &m, 
              Object3D *obj) :
        _object(obj) 
    {
		transform=m; 
	//transform=m.inverse(); //need to set variable
	//transform=m.transposed(); //need to set variable
	//transform=m.translation(0.4,0,0); //need to set variable
	//transform=(m.scaling(1,0.2,1)).inverse(); //need to set variable
    }

	//I like my print matrix method more
	void printMat(Matrix4f bob)
	{
        	cout << "Mat " << endl;
        	for(int n1=0;n1<4; n1++)
        	{
                	for(int n2=0; n2<4; n2++)
                	{
                        	cout<< bob(n1,n2) <<" ";
                	}
                	cout << endl;
        	}
        	cout << endl;
	}

    virtual bool intersect(const Ray &r, float tmin, Hit &h) const
    {
	//we have to return results with ray, transformed correctly using 4 by 4 affine transformation

	Vector3f pos,dir;
	Vector4f pos2,dir2; 

	pos=r.getOrigin();
	dir=r.getDirection();

	pos2=Vector4f(pos[0],pos[1],pos[2],1); 
	dir2=Vector4f(dir[0],dir[1],dir[2],0); 

	pos2=(transform.inverse())*pos2; 
	dir2=(transform.inverse())*dir2; 

	pos=Vector3f(pos2[0],pos2[1],pos2[2]);
	dir=Vector3f(dir2[0],dir2[1],dir2[2]);  //watch out, we need to leave this un-normalized and make 
	                                        //sure other methods using this don't normalize either
	                                        //so we can keep the math consistent 
	//dir.normalize(); //need to normalize, transformation doesnt preserve norm

	//create new transformed ray 
	Ray bob(pos,dir);

	//cout << "transform intersect " <<endl; 
        bool hello=_object->intersect(bob, tmin, h);  // TODO: implement correctly

	Matrix3f joe; 
	Vector3f norm; 
	Vector4f norm2;	

	if(hello) //transform normal
	{
		//transform.print();  //print transform for reference 

		norm=h.getNormal();
		norm2=Vector4f(norm[0],norm[1],norm[2],0); //transform as vector 
		norm2=((transform.inverse()).transposed())*norm2; //we want to give norm back in world frame, already in the object frame 
		norm=Vector3f(norm2[0],norm2[1],norm2[2]); 
		//norm=Vector3f(1,0,0); 
		norm.normalize(); 
		h.set(h.getT(),h.getMaterial(),norm); 
	}

	return hello; 
    }

  Matrix4f transform;

  protected:
    Object3D *_object; //un-transformed object	
};

#endif //TRANSFORM_H
