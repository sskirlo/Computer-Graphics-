#include "pendulumSystem.h"
#include <iostream>
#include <stdlib.h>
//#include "constants.h"

#define RES 100

#define D0 0.3 //distance between springs and elements 
#define V0 2.0 
#define KSPRING 2.0 
#define DISP 0.3
#define GRAV 0.2 
#define OFFSET -2.5

using namespace std; 

PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles)
{
	m_numParticles = numParticles;
	float vx,vy,vz; 	

	//first state is just fixed in place
	m_vVecState.push_back(Vector3f(0,0,0));
	m_vVecState.push_back(Vector3f(0,0,0));

	// fill in code for initializing the state based on the number of particles
	for (int i = 1; i < m_numParticles; i++) {
		
		// for this system, we care about the position and the velocity
		m_vVecState.push_back(Vector3f(0,-1.0*D0*i,0));
		vx=((float) (rand()%RES))/RES*V0;
		vy=((float) (rand()%RES))/RES*V0;
		vz=((float) (rand()%RES))/RES*V0;
		
		m_vVecState.push_back(Vector3f(vx,vy,vz));
	}
}


// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	// YOUR CODE HERE

	//first state is just fixed in place
	f.push_back(Vector3f(0,0,0));
	f.push_back(Vector3f(0,0,0));
	for (int i = 1; i < m_numParticles; i++) {
		f.push_back(state[1+i*2]); //position just evolves with v
		
		Vector3f force(0.0,0.0,0.0); 
		force=force-Vector3f(0,GRAV,0); //include gravity
		force=force-DISP*state[1+i*2]; //disipative force 

		//compute force from spring "above" particle

		Vector3f d=(state[0+(i*2)]-state[0+(i-1)*2]); 
		force=force-KSPRING*(d.abs()-D0)*(d.normalized()); 

		if(i!=(m_numParticles-1)) //if not the last particle on spring, include force from spring below
		{
			d=state[0+(i*2)]-state[0+(i+1)*2]; 
			force=force-KSPRING*(d.abs()-D0)*(d.normalized()); 
		}

		f.push_back(force); //velocity evolves with force 
	}

	return f;
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos=m_vVecState[0+i*2]; //velocities are other states
		glPushMatrix();
		glTranslatef(pos[0]+OFFSET, pos[1], pos[2] );
		glutSolidSphere(0.075f,10.0f,10.0f);
		glPopMatrix();
	}
}

//set external, this lets us set external movements or forces on particle system 
void PendulumSystem::setExternal(int bob)
{
        //no external inputs enabled for this system
	bob=1; 
}
