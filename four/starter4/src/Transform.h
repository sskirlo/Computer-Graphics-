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
	transform=m.inverse(); //need to set variable
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

	pos2=transform*pos2; 
	dir2=transform*dir2; 

	pos=Vector3f(pos2[0],pos2[1],pos2[2]);
	dir=Vector3f(dir2[0],dir2[1],dir2[2]);
	dir.normalize(); //need to normalize, transformation doesnt preserve norm

	//create new transformed ray 
	Ray bob(pos,dir);

	cout << "transform intersect " <<endl; 
        bool hello=_object->intersect(bob, tmin, h);  // TODO: implement correctly

	Matrix3f joe; 
	Vector3f norm;	

	if(hello) //transform normal
	{
		//transform.print();  //print transform for reference 

		joe=transform.getSubmatrix3x3(0,0); 
		joe.transpose();
		norm=h.getNormal();
		norm=joe*norm; //transform normal
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
