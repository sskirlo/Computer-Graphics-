#ifndef GROUP_H
#define GROUP_H

#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"

#include <iostream>

using namespace std; 

/// TODO: 
/// Implement Group
/// Add data structure to store a list of Object* 
class Group : public Object3D
{
  public:
    // Constructor
    Group()
    {
    }

    // Destructor
    ~Group()
    {
    }

    // Return true if intersection found
    virtual bool intersect(const Ray &r, float tmin, Hit &h) const
    {
	//cout << "new intersect " << group.size()  <<endl; 

	bool joe=false; 
        for(int i=0; i<group.size(); i++)
	{
		//cout << "checking "<< i << " of "<< group.size() <<endl;
		if(group[i]->intersect(r,tmin,h))
		{
			joe=true; 
			//tmin=h.getT(); //not supposed to update Tmin
			//cout << "hit object, t tmin " << h.getT() <<" " << tmin <<endl;
		}
	}

	//cout << "finished hitting" <<endl; 
        return joe;  
    }

	//wrong strategy, Hit contains everything we will need 

   /*
	//we need to get specific details about hitObject later for rendering 
    Object3D * getHitObject()
    {
	return hitObject; 
    }
	*/	

    // Add object to group
    void addObject(Object3D *obj)
    {
	group.push_back(obj); 
    }

    // Return number of objects in group
    int getGroupSize()
    {
        return group.size();
    }

  private:
	std::vector <Object3D*> group; 
	mutable Object3D * hitObject; //object which was hit
};

#endif
