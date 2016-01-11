//#define MAINFILE

#include "ClothSystem.h"
#include <iostream>
#include <stdlib.h>
#include <math.h>
//#include "constants.h"

#define RES 100

#define D0 0.3f //distance between springs and elements 
#define R0 1.3f
#define V0 0.5f
#define KSTRUCT 40.0f 
#define KSHEAR 20.0f 
#define KFLEX 40.0f 
#define DISP 0.47f
#define GRAV 0.3f 
#define ZMAX 5.0f //max forward Z distance we are moving
#define VCLOTH 0.5f
#define FUDGE 1.04f

using namespace std; 

//we need to be very careful with declaring global variables in c files that are called by other c files
//in general all global variables that have the same name in all files can be modified by main.cpp
//if we don't want to have this behavior, we need to use static keyword so variable will only appear within this file

//grid size, gets updated during initialization
static int SX=1;
static int SY=1;
static int flapping=0; 
static int FORWARD=1; 

//returns 0 indexed version, next poisitions correspond to velocities
static int grid(int x, int y) //returns appropriate point in list
{
	return (2*(x-1)+(y-1)*2*SX); 
}

//another grid method for making a refined mesh
static int grid2(int x, int y) //returns appropriate point in list
{
	return (2*(x-1)+(y-1)*2*(SX-1)); 
}

//assume 0 index
static int pos(int index,int * x, int * y)
{
	//figure out what x, and y we should use given index
	(*x)=((index+1)%SX); 
	if((*x)==0)
		(*x)=SX; 
	(*y)=((index+1-(*x))/SX+1); //return in 1 to SY range
}

//this will be workhorse method for computing forces between some two points in mesh
static Vector3f computeForce(vector<Vector3f> * state, int x1,int y1, int x2, int y2,float k, float dist)
{
	Vector3f d=((*state)[grid(x1,y1)]-(*state)[grid(x2,y2)]); 
  	Vector3f force=-k*(d.abs()-dist)*(d.normalized()); 

	return force;
}


//apply collisions with objects in the system, in this case we just try a sphere with R0 
static void applyCollisions(vector<Vector3f> * state)
{
	//cout << "checking collisions " <<endl;
	for(int i=0; i<((*state).size()/2); i++)
	{
		if(((*state)[2*i].abs())<(R0*FUDGE)) //find distance from the origin, if inside sphere do something 
		{
			//cout << "collision! " <<endl;
			(*state)[2*i]=R0*FUDGE*((*state)[2*i].normalized()); //just move point to surface of sphere
		}
	}
}

//TODO: Initialize here
ClothSystem::ClothSystem(int numX,int numY):ParticleSystem(numX*numY) 
{
	SX=numX;
	SY=numY; 

	m_numParticles = numX*numY;
        float vx,vy,vz;         
	int x,y; 

        // fill in code for initializing the state based on the number of particles
        for (int i = 0; i < m_numParticles; i++) {

                // for this system, we care about the position and the velocity
 
		pos(i,&x,&y); //get appropriate x,y position based on indexing scheme were using

		cout << x << " " <<y <<endl; 

                m_vVecState.push_back(Vector3f((x-1)*D0-D0*(SX*3/4),R0*1.5,(y-1)*D0+D0*0.75*SY));
                vx=((float) (rand()%RES))/RES*V0;
                vy=((float) (rand()%RES))/RES*V0;
                vz=((float) (rand()%RES))/RES*V0;

                m_vVecState.push_back(Vector3f(vx,vy,vz));
        }
}


// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
 	 vector<Vector3f> f;
        // YOUR CODE HERE

	int x,y; 

        for (int i = 0; i < m_numParticles; i++) {
                f.push_back(state[1+i*2]); //position just evolves with v

                Vector3f force(0.0,0.0,0.0); 
                force=force-Vector3f(0,GRAV,0); //include gravity
                force=force-DISP*state[1+i*2]; //disipative force 

		//compute spring forces, get position in mesh first
		pos(i,&x,&y); 

		//first lets find contributions from structural springs, need to be careful with edge cases
		if(1<x)
			force=force+computeForce(&state, x,y,x-1,y,KSTRUCT, D0);
		if(x<SX)
			force=force+computeForce(&state, x,y,x+1,y,KSTRUCT, D0);
		if(1<y)
			force=force+computeForce(&state, x,y,x,y-1,KSTRUCT, D0);
		if(y<SY)
			force=force+computeForce(&state, x,y,x,y+1,KSTRUCT, D0);

		//now we add the contributions from the shear springs 
		if(1<x && 1<y)
			force=force+computeForce(&state, x,y,x-1,y-1,KSHEAR, sqrt(2.0)*D0);
		if(1<x && y<SY)
			force=force+computeForce(&state, x,y,x-1,y+1,KSHEAR, sqrt(2.0)*D0);
		if(x<SX && 1<y)
			force=force+computeForce(&state, x,y,x+1,y-1,KSHEAR, sqrt(2.0)*D0);
		if(x<SX && y<SY)
			force=force+computeForce(&state, x,y,x+1,y+1,KSHEAR, sqrt(2.0)*D0);

		//now we add the contributions from the shear springs 
		if(2<x)
			force=force+computeForce(&state, x,y,x-2,y,KFLEX, 2.0*D0);
		if(x<(SX-1))
			force=force+computeForce(&state, x,y,x+2,y,KFLEX, 2.0*D0);
		if(2<y)
			force=force+computeForce(&state, x,y,x,y-2,KFLEX, 2.0*D0);
		if(y<(SY-1))
			force=force+computeForce(&state, x,y,x,y+2,KFLEX, 2.0*D0);

                f.push_back(force); //velocity evolves with force 
        }

	if(flapping==1)
	{
		if(ZMAX<(FORWARD*state[grid(1,1)][2])) //change direction if moved too far
		{
			FORWARD=FORWARD*-1; 
		} 

		//0 forces and velocities on points we want to constrain, hold up cloth on ends 
		f[grid(1,1)]=Vector3f(0.0,0.0,FORWARD*VCLOTH); 
		f[grid(1,1)+1]=Vector3f(0.0,0.0,0.0); 

		f[grid(SX,1)]=Vector3f(0.0,0.0,FORWARD*VCLOTH); 
		f[grid(SX,1)+1]=Vector3f(0.0,0.0,0.0); 
	}
	else
	{
		//0 forces and velocities on points we want to constrain, hold up cloth on ends 
		f[grid(1,1)]=Vector3f(0.0,0.0,0.0); 
		f[grid(1,1)+1]=Vector3f(0.0,0.0,0.0); 

		f[grid(SX,1)]=Vector3f(0.0,0.0,0.0); 
		f[grid(SX,1)+1]=Vector3f(0.0,0.0,0.0); 
	}
	
        return f;
}

///TODO: render the system (ie draw the particles)

//if we want to do collosion with sphere, we need to draw this here
void ClothSystem::draw()
{
/*
	
	//for now we just draw points, later we will render surface 
       for (int i = 0; i < m_numParticles; i++) {
                Vector3f pos=m_vVecState[0+i*2]; //velocities are other states
                glPushMatrix();
                glTranslatef(pos[0], pos[1], pos[2] );
                glutSolidSphere(0.075f,10.0f,10.0f);
                glPopMatrix();
        }
	
*/
	//define colors

	GLfloat sheetColor[] = {0.9f, 0.9f, 0.9f, 1.0f};
    	GLfloat sphereColor[] = {0.4f, 0.7f, 1.0f, 1.0f};
	
	applyCollisions(&m_vVecState); //we just need a place to apply collisions with objects we've included in simulation

	//push sphere we are colliding with onto the stack 
        glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0 );
        glutSolidSphere(R0,20.0f,20.0f);
   	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sphereColor);
        glPopMatrix(); //we pop later to make sure we are in same frame as where we defined circle to be centered
                         //doesn't make a difference anyways

	//change default color to sheet color
   	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sheetColor);

	//this method renders surface with normals which are not interpolated 

	Vector3f norm,d1,d2; 
	vector<Vector3f> norms,intnorms; 

	glDisable(GL_CULL_FACE); //general practice to not plot back face, saves half of computation
                                //here we disable this function so we can plot both sides of our surface 

	glBegin(GL_TRIANGLES);


	//draw surface, compute vertices and normals, this uncommenting will give wire-mesh
	for(int nx=1; nx<SX; nx++)
	{
		for(int ny=1; ny<SY; ny++)
		{
			//draw two triangles for each cell

			//define normal
			d1=m_vVecState[grid(nx+1,ny)]-m_vVecState[grid(nx,ny)];
			d2=m_vVecState[grid(nx+1,ny+1)]-m_vVecState[grid(nx,ny)];
			norm=(norm.cross(d1,d2)).normalized();
			norms.push_back(norm);

			
	        	//glNormal(norm);
        		//glVertex(m_vVecState[grid(nx,ny)]);
	        	//glNormal(norm);
        		//glVertex(m_vVecState[grid(nx+1,ny)]);
	        	//glNormal(norm);
        		//glVertex(m_vVecState[grid(nx+1,ny+1)]);
			

			//define normal
			d1=m_vVecState[grid(nx+1,ny+1)]-m_vVecState[grid(nx,ny)];
			d2=m_vVecState[grid(nx,ny+1)]-m_vVecState[grid(nx,ny)];
			norm=(norm.cross(d1,d2)).normalized();
			norms.push_back(norm); 

			
	        	//glNormal(norm);
        		//glVertex(m_vVecState[grid(nx,ny)]);
	        	//glNormal(norm);
        		//glVertex(m_vVecState[grid(nx+1,ny+1)]);
	        	//glNormal(norm);
        		//glVertex(m_vVecState[grid(nx,ny+1)]);
			
		}
    	}

	//computes interpolated normals for each vertex
	for(int nx=1; nx<=SX; nx++)
	{
		for(int ny=1; ny<=SY; ny++)
		{
			//draw two triangles for each cell

			//compute set of 
			
			Vector3f norm1(0.0,0.0,0.0); 

			//compute interpolated normals for each vertex 
			if(nx>1 && ny>1)
			{
				norm1=norm1+norms[grid2(nx-1,ny-1)];
				norm1=norm1+norms[grid2(nx-1,ny-1)+1];
			}
			if(nx<(SX-1) && ny<(SY-1))
			{
				norm1=norm1+norms[grid2(nx+1,ny+1)];
				norm1=norm1+norms[grid2(nx+1,ny+1)+1];
			}
			if(nx<(SX-1) && ny>1)
			{
				norm1=norm1+norms[grid2(nx+1,ny-1)+1];
			}
			if(ny<(SY-1) && nx>1)
			{
				norm1=norm1+norms[grid2(nx-1,ny+1)];
			}
			
			//push two on so index the same way as our other list
			intnorms.push_back(norm1.normalized()); 
			intnorms.push_back(norm1.normalized()); 
		}
    	}

	//finally with the interpolated norms we can render a smoothed mesh
	for(int nx=1; nx<SX; nx++)
	{
		for(int ny=1; ny<SY; ny++)
		{
			//draw two triangles for each cell

	        	glNormal(intnorms[grid(nx,ny)]);
        		glVertex(m_vVecState[grid(nx,ny)]);
	        	glNormal(intnorms[grid(nx+1,ny)]);
        		glVertex(m_vVecState[grid(nx+1,ny)]);
	        	glNormal(intnorms[grid(nx+1,ny+1)]);
        		glVertex(m_vVecState[grid(nx+1,ny+1)]);
			
	        	glNormal(intnorms[grid(nx,ny)]);
        		glVertex(m_vVecState[grid(nx,ny)]);
	        	glNormal(intnorms[grid(nx+1,ny)]);
        		glVertex(m_vVecState[grid(nx+1,ny+1)]);
	        	glNormal(intnorms[grid(nx,ny+1)]);
        		glVertex(m_vVecState[grid(nx,ny+1)]);
			
		}
    	}

    	glEnd();

	glEnable(GL_CULL_FACE); //reenable culling faces
        
	//glPopMatrix();	

}

//set external, this lets us set external movements or forces on particle system 
void ClothSystem::setExternal(int bob)
{
	cout << "set flapping state " << bob <<endl; 	
	flapping=bob;
}

