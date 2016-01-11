#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>
#include <iostream>

using namespace std; 

#include "Hit.h"
#include "Ray.h"
#include "Texture.h"
//#include "SceneParser.h"
//#include "Light.h"

//we need to forward declare classes, otherwise we get circular dependence 
//this doesn't work

// TODO: Implement Shade function that uses ambient, diffuse, specular and texture
class Material
{
  public:
    Material(const Vector3f &diffuseColor,
             const Vector3f &specularColor = Vector3f::ZERO, 
             float shininess = 0) :
        _diffuseColor(diffuseColor),
        _specularColor(specularColor), 
        _shininess(shininess),
	loaded(false)
    {
    }

    virtual ~Material()
    {
    }

    virtual const Vector3f & getDiffuseColor() const 
    { 
        return _diffuseColor;
    }

    virtual const Vector3f & getSpecularColor() const 
    { 
        return _specularColor;
    }

    virtual const float & getShininess() const 
    { 
        return _shininess;
    }

    virtual Texture * getTexture() //we return texture so renderer shade method can do texturing for triangles   
    { 
        return (&(_texture));
    }

	/*
	//we moved implementation to main loop, couldn't use light and scene in this method like I wanted
    Vector3f shade(const Ray &ray, const Hit &hit, SceneParser * Scene ) 
    {
	//first we need to figure out where this section of material is located 
	Vector3f n=hit.getNormal(); 
	Vector3f dray=ray.getDirection(); 
	Vector3f p=ray.pointAtParameter(hit.getT());
	Vector3f c_amb=Scene->getAmbientLight();
	int num=Scene->getNumLights(); 

	Vector3f color=_diffuseColor*c_amb; // is direct vector product

	Light * bob;
	Vector3f ldir, col,ref; 
	float dist,val,val2; 

	//iterate through light sources 
	for(int i=0; i<num; i++)
	{
		bob=Scene->getLight(i);
		bob->getIllumination(&p,&ldir,&col,&p); 
		
		//compute contribution from diffuse reflection
		val=ldir.dot(ldir,n); 
		if(val<0) //clamp value
			val=0; 		
		color=color+_diffuseColor*col*val; 
		
		//compute contribution from specular reflection
		ref=-ldir+2*ldir.dot(ldir,n)*n; 
		val2=pow(dray.dot(dray,n),_shininess); 
		if(val<0) //clamp value
			val2=0; 
 
		color=color+_specularColor*col*val2;	
	}

        return color; 
    }
	*/

    void loadTexture(const std::string &filename)
    {
	cout << "loaded texture into material " <<endl; 
        _texture.load(filename);
	loaded=true;
	name=filename; //save for later 
    }

   bool isLoaded()
   {
	cout << name <<endl;
	return loaded;
   }

  private:
    Vector3f _diffuseColor;
    Vector3f _specularColor;
    float _shininess;
    Texture _texture;
    bool loaded; 
    string name;
};

#endif // MATERIAL_H
