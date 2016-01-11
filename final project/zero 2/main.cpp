#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <vecmath.h>
using namespace std;

#define MAX_BUFFER 100

// Globals

float timertime=200; 
int zoomsense=200;
int rotatesense=150; 


int rotatephi=0; 
int rotatetheta=0; 
int mouseon=0; 
int mousezoom=0; 
int mouserotate=0; 

int color=1; 

float Rad=5.0; 
float cameraRad=Rad; 

int mousex;
int mousey; 

float theta0=0.0;
float phi0=0.0; 

float thetastore=0.0; 
float phistore=0.0; 

float inctheta=0.1; 
float incphi=0.1; 

float posx=1.0; 
float posy=1.0;
float posz=5.0;  

float incx=0.5; 
float incy=0.5; 

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;


// You will need more global variables to implement color and position changes


// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }

//timer functions which get triggered as we spin the object, we can trigger each angle individually or both

void timer(int id) {
	if(id==1)
		theta0=theta0+inctheta; 
	if(id==2)
		phi0=phi0+incphi; 
   	glutPostRedisplay();
}

//gets mouse position when mouse moves inside window
//will only do something if m is pressed, pressing m again will disable function
//if left mouse button is clicked down, we record position, and update mouse position as we rotate, releasing will lock object
//if right mouse button is clicked down, moving left will zoom in, moving right will zoom out, releasing will lock zoom
//if neither mouse button is clicked, nothing will happen 

void mouseUpdateClick(int button, int state, int x, int y)
{
	if(mouseon==1)  //only do something if mouse state is enabled 
	{
		mousex=x;  //set mouse position state, we will zoom and rotate based on initial and current positions
		mousey=y; 
		cout << "x,y mouse position, button, state " << mousex << " " << mousey << " " << button <<" " << state << endl;

		//update variables controlling mouse zoom/ mouse rotate state 
		if(state==GLUT_DOWN)
		{
			if(GLUT_LEFT_BUTTON==button)
			{
				mouserotate=1;
				thetastore=theta0; 
				phistore=phi0;  
			}
			else
			{
				mousezoom=1; 
			}
		}
		else
		{
			if(GLUT_LEFT_BUTTON==button)
			{
				mouserotate=0; 
			}
			else
			{
				mousezoom=0; 
			}
		}
	}
}

//depending on 
void mouseUpdateMove(int x, int y)
{
	if(mouseon==1)  //only do something if mouse state is enabled 
	{
		//cout << "x,y mouse position, rotate, zoom " << mousex << " " << mousey << " " << mouserotate << " " << mousezoom << endl; 
		if(mouserotate==1)
		{
			theta0=thetastore+ ((float)(mousey-y)/rotatesense); 
			phi0=phistore+ ((float)(mousex-x)/rotatesense); 			
		}
		if(mousezoom==1)
		{
			cameraRad=Rad*( ((float)(x-mousex)/zoomsense)+1);  //zoom in/out based on x position
		}
	   	glutPostRedisplay();
	}
}

// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
	//	cout << "Unhandled key press " << key << "." << endl; 

	//lets just print color number
	cout << "Color " << color++ << endl; 	

	break;
	//enable mouse function, press again to disable mouse function
    case 'm':
	mouseon=(mouseon+1)%2; 
	cout << "mouse state " << mouseon << endl; 	

	break;
	//update phi angle
    case 'r':
	rotatephi=(rotatephi+1)%2; 
        // put animation in rotating state, if we press r again, disable this state
	cout << "rotate " << rotatephi << endl; 	

	//just make sure we can get camera to rotate first 
	//phi0=phi0+incphi; 
	
	break;
	//rotate theta angle
    case 't':
	rotatetheta=(rotatetheta+1)%2; 
        // put animation in rotating state, if we press r again, disable this state
	cout << "rotate " << rotatetheta << endl; 	

	//just make sure we can get camera to rotate first 
	//theta0=theta0+inctheta; 


        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // add code to change light position
		//cout << "Unhandled key press: up arrow." << endl;
		posy=posy+incy; 
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
		//cout << "Unhandled key press: down arrow." << endl;
		posy=posy-incy; 
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
		//cout << "Unhandled key press: left arrow." << endl;
		posx=posx-incx; 
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
		//cout << "Unhandled key press: right arrow." << endl;
		posx=posx+incx; 
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}


// uses global variable color which is updated by keyBoard func to set the color redrawn
// This function is responsible for displaying the object.
void drawScene(void)
{
 
	float x,y,z;   

   int i;

	//after we call rotate keys, we will call here, this will start timer and draw scene once 
	//without any changes, the timer will then modify angle variables, and then call this function again...
	//and then update the scene, we stop the rotation by pressing the key again and changing state 
	if(rotatetheta==1)
	{
		glutTimerFunc(timertime,timer,1);  
	}
	if(rotatephi==1)
	{
		glutTimerFunc(timertime,timer,2);  
	}




    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.

 	x=sin(phi0)*cos(theta0)*cameraRad;
	y=sin(phi0)*sin(theta0)*cameraRad;
	z=cos(phi0)*cameraRad; 

    gluLookAt(x, y, z,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // Set material properties of object

	// Here are some colors you might use - feel free to add more
    GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0} };
    
	// Here we use the first color entry as the diffuse color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[color % 4]);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
	//GLfloat Lt0pos[] = {1.0f, 1.0f, 5.0f, 1.0f};
	GLfloat Lt0pos[] = {posx, posy, posz, 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	
	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	//glutSolidTeapot(1.0);
	    
 //now we draw object we roaded 

	int len=vecf.size();	
	int a,b,c,d,e,f,g,h; 

	for(int n=0; n<len; n++)
	{
		a=vecf[n][0]; 
		b=vecf[n][1]; 
		c=vecf[n][2]; 
		d=vecf[n][3]; 
		e=vecf[n][4]; 
		f=vecf[n][5]; 
		g=vecf[n][6]; 
		h=vecf[n][7]; 
		i=vecf[n][8]; 
		
		glBegin(GL_TRIANGLES);
		glNormal3d(vecn[c-1][0], vecn[c-1][1], vecn[c-1][2]);
		glVertex3d(vecv[a-1][0], vecv[a-1][1], vecv[a-1][2]);
		glNormal3d(vecn[f-1][0], vecn[f-1][1], vecn[f-1][2]);
		glVertex3d(vecv[d-1][0], vecv[d-1][1], vecv[d-1][2]);
		glNormal3d(vecn[i-1][0], vecn[i-1][1], vecn[i-1][2]);
		glVertex3d(vecv[g-1][0], vecv[g-1][1], vecv[g-1][2]);
		glEnd();
	}



    // Dump the image to the screen.
    glutSwapBuffers();


}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

//writes vector to output stream
int printVec(Vector3f vec)
{
	cout << vec[0] << " " << vec[1] << " " <<vec [2] << endl; 
	return 0; 
}

//writes face to output stream
int printFace(vector<unsigned> joe)
{
	for(int n=0; n<9; n++)
		cout << joe[n] << " "; 
	cout << endl; 
  
	return 0; 
}
//we read in the file specified in cin stream
void loadInput()
{
	Vector3f bob;
	vector<unsigned> joe; 
	string s; 

	//initialize vectors for reading in verticies, faces, and normals 

	//vector<Vector3f> vecv; 
	//vector<Vector3f> vecn; 
	//vector<int9> vecf; 

	cout << "loading input" << endl; 

	char buffer[MAX_BUFFER];
	string buffer2; 

	if(!cin.getline(buffer,MAX_BUFFER))
	{
		cout << "no input file available" << endl; 
		return; 
	}


	//str(char array) makes printable string out of char array

	//clear out first 4 lines
	for(int x=0; x<2; x++)
	{
		cin.getline(buffer,MAX_BUFFER);
		cout << string(buffer) << endl; //just print out for checking 
	}	

	
	// load the OBJ file here
	while(cin.getline(buffer,MAX_BUFFER))  //returns false when we are at end of file
	{
		stringstream ss(buffer); 
		ss >> s; 
 
		//case if vector type
		if(s == "v")
		{
			ss >> bob[0] >> bob[1] >> bob[2];  
			vecv.push_back(bob); 
		}
		if(s=="vn")
		{
			ss >> bob[0] >> bob[1] >> bob[2];  
			vecn.push_back(bob); 
		}
		if(s=="f")
		{
			//face type
			for(int n=0; n<9; n++)
			{
				if(n%3==2)  //alternate in token we use to read through buffer
					getline(ss, buffer2,' ');   
				else
					getline(ss, buffer2, '/'); 
				joe.push_back(atoi(buffer2.c_str()));  //can also use stringstream to convert to integer  
			}
			//printFace(joe); 
			vecf.push_back(joe);  //pushback joe and then pushback onto vecf 
			joe.clear(); //clear out didnt do that before ;) 
		}
	}

	//print out sizes of each of the lists we've read in
	cout << "vertex, normal, face " << vecv.size() << " " << vecn.size() << " " << vecf.size() << endl; 	
	
	//want to check that first and last entries are correct for vecs, normals, faces, so lets print out

	/*
	//verify each array is properly read in 
	printVec(vecv[0]);
	printVec(vecv[vecv.size()-1]);	
	printVec(vecn[0]);
	printVec(vecn[vecn.size()-1]);	
	printFace(vecf[0]);
	printFace(vecf[vecf.size()-1]);	
	*/

}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

	//set callback for trigger on mouse movement events, one of the mouse buttons has to be pressed 
	glutMouseFunc(mouseUpdateClick);

	//update state when mouse is already pressed and we are moving around
	glutMotionFunc(mouseUpdateMove);


     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
