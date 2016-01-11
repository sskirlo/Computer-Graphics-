

#include "simpleSystem.h"

#define OFFSET -2.5

using namespace std;

SimpleSystem::SimpleSystem()
{
	//initialize state and number of particles 
	m_numParticles=1; 
	m_vVecState.push_back(Vector3f(1.0,0,0));
}


//this should just create a particle going in a circle 
// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> SimpleSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	f.push_back(Vector3f(-state[0][1],state[0][0],0)); 

	return f;
}

// render the system (ie draw the particles)
void SimpleSystem::draw()
{
		Vector3f pos=m_vVecState[0]; //only one state in array
		  glPushMatrix();
		glTranslatef(pos[0]+OFFSET, pos[1], pos[2] );
		glutSolidSphere(0.075f,10.0f,10.0f);
		glPopMatrix();
}

//set external, this lets us set external movements or forces on particle system 
void SimpleSystem::setExternal(int bob)
{ 
       bob=1;
}


