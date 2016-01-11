#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std; 

// TODO: implement this class.
class Triangle: public Object3D
{
  public:
    Triangle(const Vector3f &a, 
             const Vector3f &b, 
             const Vector3f &c, 
             const Vector3f &na,
             const Vector3f &nb,
             const Vector3f &nc,
             Material *m) :
        Object3D(m),
        _hasTex(false)
    {
            _vertices[0]=a;
            _vertices[1]=b;
            _vertices[2]=c;
            _normals[0]=na;
            _normals[1]=nb;
            _normals[2]=nc;
    }

//const at end of function means we are not allowed to modify class members unless we 
//make specific class members "mutable", this is a keyword I haven't encountered before 

    virtual bool intersect(const Ray &ray, float tmin, Hit &hit) const  {
        //need to implement intersection with triangle

	Vector3f O=ray.getOrigin();
	Vector3f D=ray.getDirection();

	Vector3f a=_vertices[0];
	Vector3f b=_vertices[1];
	Vector3f c=_vertices[2];

	Matrix3f A(a[0]-b[0], a[0]-c[0], D[0],
	           a[1]-b[1], a[1]-c[1], D[1],
	           a[2]-b[2], a[2]-c[2], D[2]); 

	Matrix3f B(a[0]-O[0], a[0]-c[0], D[0],
	           a[1]-O[1], a[1]-c[1], D[1],
	           a[2]-O[2], a[2]-c[2], D[2]); 

	Matrix3f G(a[0]-b[0], a[0]-O[0], D[0],
	           a[1]-b[1], a[1]-O[1], D[1],
	           a[2]-b[2], a[2]-O[2], D[2]); 

	Matrix3f T(a[0]-b[0], a[0]-c[0], a[0]-O[0],
	           a[1]-b[1], a[1]-c[1], a[1]-O[1],
	           a[2]-b[2], a[2]-c[2], a[2]-O[2]); 

	float beta=B.determinant()/A.determinant();
	float gamma=G.determinant()/A.determinant();
	float t=T.determinant()/A.determinant();

	//cout << "beta, gamma, t" << beta << gamma << t <<endl; 

	if(t<hit.getT() && t>tmin)
	{
		//cout << "hit " << beta << " " << gamma <<endl; 

		if((beta+gamma)>1 || (beta<0)  || (gamma<0) )
			return false; //doesn't hit triangle
		else
		{
			//cout << "hit " << beta << " " << gamma << " " << t <<endl; 

			float alpha=(1-beta-gamma); 

			//interpolate normals
			Vector3f norm=alpha*_normals[0]+beta*_normals[1]+gamma*_normals[2]; 
			norm.normalize(); //normalize
			hit.set(t,getMaterial(),norm);  //need to fill in material part  
		
			if(_hasTex)
			{
				if(!(getMaterial()->isLoaded()))
					cout << "tex file is not loaded even though we have tex coordinates!" <<endl; 
				//else
					//cout << "Material is loaded" <<endl; 

				//if hasTex we store coordinates for texture 
				//we need to store this in hit object 
				Vector2f uv=(alpha*_texCoords[0]+beta*_texCoords[1]+gamma*_texCoords[2]);
				hit.setTexCoord(uv); 				
			}
			else
			{
				hit.setTexFalse(); //need to set Hit state to be false
			}
	
			return true; //missed this before lol
		}
	}
	else
	{
		return false;
	}  
	return false; 
    }

    void setTex(const Vector2f &uva,
                const Vector2f &uvb,
                const Vector2f &uvc)
    {
        _texCoords[0] = uva;
        _texCoords[1] = uvb;
        _texCoords[2] = uvc;
        _hasTex = true;
    }

    bool hasTex() const {
        return _hasTex;
    }

  private:
    bool _hasTex;
    Vector3f _vertices[3];
    Vector3f _normals[3];
    Vector2f _texCoords[3];
};

#endif //TRIANGLE_H
