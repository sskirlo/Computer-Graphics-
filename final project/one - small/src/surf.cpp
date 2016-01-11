#include "surf.h"
#include "extra.h"
#include "math.h"

using namespace std;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

//make copy of curve object
void copyCurve(const Curve &bob,Curve * copy)
{
	CurvePoint joe; 
	for(int n=0; n<bob.size(); n++)
	{
		joe.V=Vector3f(bob[n].V);
		joe.T=Vector3f(bob[n].T);
		joe.N=Vector3f(bob[n].N);
		joe.B=Vector3f(bob[n].B);
		(*copy).push_back(joe);
	}
}

/*
//delete curve, dont need anymore once function is finished
void deleteCurve(Curve * bob)
{
	~(*bob).V;
	~(*bob).T;
	~(*bob).N;
	~(*bob).B;
}
*/

//rotate curve, angle is in radians
void rotateCurve(Curve * bob, float angle)
{
	Matrix3f rotate(cos(angle),0,sin(angle),
                        0         ,1,      0,
                        -sin(angle),0,cos(angle));
	
	//Curve joe=(*bob);

	for(int n=0; n<(*bob).size(); n++)
	{
		(*bob)[n].V=rotate*(*bob)[n].V;
		(*bob)[n].N=rotate*(*bob)[n].N; 
	}
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

	float pi=3.14159;
	float dtheta=(2.0*pi)/steps; 

	cout << "theta step size " << dtheta <<endl; 

	Curve copy;
	copyCurve(profile,&copy);

	int vertexCount=0; //index for vertices as we add them

	/*
	//print out vertices for debugging
	for(int n2=0; n2<copy.size(); n2++)
		cout << copy[n2].V[0] << " " << copy[n2].V[1] << " " << copy[n2].V[2] << endl; 
	*/

	int len=copy.size();

	//generate vertices and normals 
	for(int n=0; n<(steps);n++)
	{
		//add vertices and normals for curve to surface
		for(int n2=0; n2<copy.size(); n2++)
		{
			surface.VV.push_back(copy[n2].V);
			surface.VN.push_back(copy[n2].N);
		}

		//define faces
		if(n!=(steps-1))
		{
			for(int n2=0; n2<(copy.size()-1); n2++)
			{
				surface.VF.push_back(Tup3u(n2+n*len, n2+1+n*len , n2+1+(n+1)*len) );  //lower triangle
				surface.VF.push_back(Tup3u(n2+n*len, n2+1+(n+1)*len , n2+(n+1)*len) );  //upper triangle
			}
		}
		else  //handle end curve differently, need to be careful with indexing
		{
			for(int n2=0; n2<(copy.size()-1); n2++)
			{
				surface.VF.push_back(Tup3u(n2+n*len, n2+1+n*len , n2+1+0*len) );  //lower triangle
				surface.VF.push_back(Tup3u(n2+n*len, n2+1+0*len , n2+0*len ) );  //upper triangle
			}		
		}

		//rotate curves to next positions
		rotateCurve(&copy,dtheta); 	
	}	

	
	while(copy.size()>0)
		copy.pop_back();

	//copy.erase(0,(int)(copy.size()-1)); //delete copy, destructor ~copy() doesnt seem to work...



    // TODO: Here you should build the surface.  See surf.h for details.

//    cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;


 
    return surface;
}

//method for translating and rotating profile curve 

void affineCurve(Curve * target, const Curve * start, CurvePoint pos)
{
	Matrix3f rotate(pos.N[0],pos.B[0],pos.T[0],
                        pos.N[1],pos.B[1],pos.T[1],
                        pos.N[2],pos.B[2],pos.T[2]);
	
	//Curve joe=(*bob);

	for(int n=0; n<(*start).size(); n++)
	{
		//rotate key variables
		(*target)[n].V=rotate*(*start)[n].V;
		(*target)[n].N=rotate*(*start)[n].N; 
		//translate vertex
		(*target)[n].V=(*target)[n].V+pos.V; 
	}
}


Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

	Curve copy;
	copyCurve(profile,&copy);


	/*
	//print out vertices for debugging
	for(int n2=0; n2<copy.size(); n2++)
		cout << copy[n2].V[0] << " " << copy[n2].V[1] << " " << copy[n2].V[2] << endl; 
	*/

	int sweeplen=sweep.size();
	int len=copy.size();

	//generate vertices and normals 
	for(int n=0; n<sweeplen;n++)
	{
		//rotate and translate curve into the correct position
		affineCurve(&copy,&profile,sweep[n]); 	
		
		//add vertices and normals for curve to surface
		for(int n2=0; n2<copy.size(); n2++)
		{
			surface.VV.push_back(copy[n2].V);
			surface.VN.push_back(copy[n2].N);
		}

		//define faces
		if(n!=(sweeplen-1))
		{
			for(int n2=0; n2<(copy.size()-1); n2++)
			{
				surface.VF.push_back(Tup3u(n2+n*len, n2+1+n*len , n2+1+(n+1)*len) );  //lower triangle
				surface.VF.push_back(Tup3u(n2+n*len, n2+1+(n+1)*len , n2+(n+1)*len) );  //upper triangle
			}
		}
		else  //handle end curve differently, need to be careful with indexing
		{
			for(int n2=0; n2<(copy.size()-1); n2++)
			{
				surface.VF.push_back(Tup3u(n2+n*len, n2+1+n*len , n2+1+0*len) );  //lower triangle
				surface.VF.push_back(Tup3u(n2+n*len, n2+1+0*len , n2+0*len ) );  //upper triangle
			}		
		}

	}	

	
	while(copy.size()>0)
		copy.pop_back();



    // TODO: Here you should build the surface.  See surf.h for details.

    //cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." <<endl;

    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
