#include "TimeStepper.h"
#include <iostream>

using namespace std; 

///TODO: implement Explicit Euler time integrator here
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	//cout << "entering " <<endl; 

	vector<Vector3f> curState=(*particleSystem).getState();
	vector<Vector3f> divState=(*particleSystem).evalF(curState);

	//cout << "got state and derivative " <<endl; 

	for(int n=0; n<(curState.size()); n++)
	{
		curState[n]=curState[n]+stepSize*divState[n]; 
	}

	(*particleSystem).setState(curState); 
}

///TODO: implement Trapzoidal rule here
void Trapzoidal::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	vector<Vector3f> curState=(*particleSystem).getState();
	vector<Vector3f> curState2=(*particleSystem).getState();
	vector<Vector3f> futState; 
	vector<Vector3f> dS1=(*particleSystem).evalF(curState);

	for(int n=0; n<(curState.size()); n++)
	{
		curState2[n]=curState[n]+stepSize*dS1[n]; 
	}

	vector<Vector3f> dS2=(*particleSystem).evalF(curState2);
	
	for(int n=0; n<(curState.size()); n++)
	{
		futState.push_back(curState[n]+stepSize/2*(dS1[n]+dS2[n])); 
	}
	
	(*particleSystem).setState(futState); 
}
