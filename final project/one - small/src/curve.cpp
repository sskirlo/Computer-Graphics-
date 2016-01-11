#include "curve.h"
#include "extra.h"
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }

    
}
    

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.


Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit( 0 );
    }

/*	
    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i][0] << " " << P[i][1] <<" " << P[i][2]  << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning empty curve." << endl;
*/

	Vector4f controlPoints[3]; //we will populate with control points for Bezier curve

	Vector4f tvector; 
	Vector4f bvector; 
	Vector3f position; 
	Vector3f p; 
	Vector3f zero(0.0,0.0,0.0); //also can rewrite this as vector3f zero=vector3f(...) or vector3f zero=new vector3f(..), if we don't want to allocate from the heap

	Matrix4f convertBasis(1.0,-3.0,3.0,-1.0,
                              0.0,3.0,-6.0,3.0, 
                              0.0,0.0,3.0,-3.0,
                              0.0,0.0,0.0,1.0);

	float t=0.0; 
	int segments=(P.size()-1)/3; 	
	int segstep=steps; //calculate approximately number of steps on each segment, last segment will have an extra point, so symmetric
	int range=segstep; 
	int step=0; //step along R-curve
	int len=segstep*segments+1; 


    	Curve R( len ); //we put at least int step per segment

	//find number of sub-curves , we will cycle t from 0 to 1 independently on each curve

	//use control points to define Bezier curve
	for(int n=0; n<segments;n++)
	{
		if(n==segments-1)
			range=segstep+1; //add extra point for last segment	

		//cout << "range " << range << " segments " << segments << endl; 

		//pick control points we are using for segment
		
		for(int a=0; a<3; a++)
		{
			for(int b=0; b<4; b++)
			{
				//assert((b+n*3)<P.size()); 				
				controlPoints[a][b]=P[b+n*3][a]; 
			}
		} 

		for(int n1=0;n1<range;n1++)
		{
			t=((float)n1)/(segstep);//need to cast as float  
			tvector=Vector4f(1,t,t*t,t*t*t);
			bvector=convertBasis*tvector; //convert to Bezier basis
			
			for(int a=0; a<3; a++)
			{
				position[a]=controlPoints[a].dot(controlPoints[a],bvector);   //n eed to figure out how to call dot method
			}
			//cout << t << " " << position[0] << " " << position[1] << " " << position[2] << endl; 
			R[step].V=position;  
			R[step].T=zero;
			R[step].N=zero;
			R[step].B=zero;
			 
			step++; 
		}
	}

	cout << "Number of steps" << step << endl; 

	//check if control points are 2D
	bool is2D=true; 
	for(int n=0; n<P.size();n++)
	{
		if((P[n][2]>0.01) || (P[n][2]<-0.01))
		{
			//cout << "is not 2d" <<endl; 
			is2D=false;
			break;
		}
	}

	if(is2D)
		cout << "is 2d" <<endl; 
		

	//cout << "length " << len << endl;

	//define tangent, normal, and binormal vectors for curve
	for(int n=0; n< len; n++)
	{
		if(n==0) //special case for first vectors on line
		{
			position=(R[n+1].V-R[n].V);
			R[n].T=(position.normalized()); //normalize position, and then set as tangent vector
			
			if(is2D==true)
				R[n].B=Vector3f(0.0,0.0,-1.0);  //pick z-direction for B if 2D case, rest of code is same  
			else
				R[n].B=(p.cross(R[n].T,Vector3f(1.0,1.3,1.1))).normalized();  //we initialize B with a random vector 
										//normal to T
			R[n].N=(p.cross(R[n].B,R[n].T)).normalized(); 
		}
		else
		{
			position=(R[n].V-R[n-1].V);
			R[n].T=(position.normalized()); //normalize position, and then set as tangent vector

			R[n].N=(p.cross(R[n-1].B,R[n].T)).normalized();  //use old B to find new N 
			
			R[n].B=(p.cross(R[n].T,R[n].N)).normalized();  //find new B with new N
		}
	}
	



    // Right now this will just return this empty curve.
	return R;
}


    // TODO:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.

//I like my method more 
int printMat(Matrix4f bob)
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



Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }


	Vector4f controlPoints[3]; //we will populate with control points for Bezier curve

	Matrix4f convertToBz(1.0,-3.0,3.0,-1.0,
                                0.0,3.0,-6.0,3.0, 
                                0.0,0.0,3.0,-3.0,
                                0.0,0.0,0.0,1.0);

	Matrix4f convertToBs(1.0/6.0,-3.0/6.0,3.0/6.0,-1.0/6.0,
                             4.0/6.0,0.0/6.0,-6.0/6.0,3.0/6.0, 
                             1.0/6.0,3.0/6.0,3.0/6.0,-3.0/6.0,
                             0.0/6.0,0.0/6.0,0.0/6.0,1.0/6.0);

	//that didn't work the way I thought it did...annoying
	//cout << "check scaling " <<endl;
	//printMat(convertToBs);
	//convertToBs=convertToBs.uniformScaling(1.0/6.0);
	//convertToBs.print();
	//printMat(convertToBs); 
	
	Matrix4f fromBz=convertToBz.inverse(); 
	Matrix4f prod=convertToBs*fromBz; //multiply once and reuse

	//transpose works
	//cout << "check transpose " <<endl;
	//printMat(prod); 
	prod.transpose();  //need to transpose b/c operator is only properly defined for 4 by 4 times 4 by 1, not 1 by 4 times 4 by 4
	//printMat(prod); 

	//inverse works
	/*
	//check inverse
	Matrix4f inverse=fromBz*convertToBz;
	printMat(inverse); 
 	*/

	//we have to translate every set of 4 points into the appropriate set of Bezier points. 
	//e.g. 1,2,3,4 and then 2,3,4,5 if there are 5 points total
	//we get 1', 2',3',4' and 2'',3'',4'',5''
	//4'=2'' if we did transformation correctly (we confirmed this property
	//last step is to join properly, need to overwrite 4' b/c Bezier algorithm already assumes in 4 set that end=start etc
	// 1',2',3',2'',3'',4'',5''
        //since B-spline is continuous, and endpoints of Bezier are the curves themselves, 4-prime=2-prime, 
        //which we can use as a test to verify the transformations are working properly 

	vector<Vector3f> Ptransform; 

	int segments=P.size()-3; //every single point we add after 4, we create a new segment for Bezier 


	for(int n=0; n<segments; n++)
	{

		//read into matrix which we will apply transformations to for a set of 4 control points 
		for(int a=0; a<3; a++)
		{
			for(int b=0; b<4; b++)
			{
				//assert((b+n*3)<P.size()); 				
				controlPoints[a][b]=P[b+n][a]; 
			}
		} 

		//convert control point basis
		for(int a=0; a<3; a++)
		{
			controlPoints[a]=prod*controlPoints[a];  
		}

		//read out of matrix into P-transformed vector 

		//read into matrix which we will apply transformations to for a set of 4 control points 
		for(int b=0; b<4; b++)
		{
			Ptransform.push_back(Vector3f(controlPoints[0][b],controlPoints[1][b],controlPoints[2][b])); 
		} 
		if(n!=(segments-1)) //if not last segment
		{
			Ptransform.pop_back(); //pop off last element of group of 4, 
                                          //will be equivalent to first element of next group of 4, checked 
		}
	}

	//check size of final vector and make sure its 3n+1
	cout << "Size " << P.size() << " new size " << Ptransform.size() <<endl;  

	//we checked that endpoints of each sets of 4 points matched, then we rewrote to cut these elements off 
	/*	
	//lets print out control points and make sure they make sense with what we expect, first and last points of 
	//sets of 4 should match b/c transformation should preserve continuity of B-spline

	cout << "original " <<endl; 
	//print original control points
	for(int n=0; n<P.size();n++)
	{
		cout << n << " " << P[n][0] << " " << P[n][1] << " " << P[n][2] << endl; 
	}

	cout << "new " <<endl; 

	for(int n=0; n<Ptransform.size();n++)
	{
		cout << n << " " << Ptransform[n][0] << " " << Ptransform[n][1] << " " << Ptransform[n][2] << endl; 
	}
	*/

	Curve R; 

	R=evalBezier(Ptransform,steps); 

	//now we transform into B-spline basis 


	/*
    //Curve R( steps+1 );

    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i][0] << " " << P[i][1] <<" " << P[i][2]  << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning empty curve." << endl;
	*/


    // Return an empty curve right now.
    return R;
}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

	//cout << curve.size() <<endl; 
        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

	//display vectors for debugging 
	    //cout << i << " " << curve[i].T[0] << " " << curve[i].T[1] << " " << curve[i].T[2] << endl; 


            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

