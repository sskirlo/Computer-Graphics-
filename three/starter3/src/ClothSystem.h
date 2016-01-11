#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include "extra.h"
#include <vector>

#include "particleSystem.h"

class ClothSystem: public ParticleSystem
{
///ADD MORE FUNCTION AND FIELDS HERE
public:
	ClothSystem(int sx, int sy);

	vector<Vector3f> evalF(vector<Vector3f> state);

	void draw();

	void setExternal(int bob); 

private:

	

};


#endif
