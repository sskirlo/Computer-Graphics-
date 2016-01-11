#include "SkeletalModel.h"
#include <iostream>
#include <sstream>

#include <FL/Fl.H>

#define MAX_BUFFER 400

using namespace std;

int parent=0; //use to keep track of where we are in tree

// pointer to the root joint
Joint* m_rootJoint;
// the list of joints.
std::vector< Joint* > m_joints;

Mesh m_mesh;

MatrixStack m_matrixStack;


void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	//cout << "pushing camera matrix" <<endl; 
	m_matrixStack.push(cameraMatrix);
	//m_matrixStack.push(cameraMatrix);
	//m_matrixStack.top();

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();

		//cout << "drew skeleton " <<endl; 
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}


//I like my print matrix method more 
void printMat(Matrix4f bob)
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


//goes through and gives information stored in tree, starting with root node, we use this to validate struture, 
//returns self and nodes below
int displayTree(Joint * bob)
{
	cout << "children " << ((*bob).children).size() << endl; 
	printMat((*bob).transform);	

	int tot=0; 

	for(int n=0; n<((*bob).children).size(); n++)
		tot=tot+displayTree((*bob).children[n]);  //go through children  	

	return tot+1; //returns number of leaves below including self 
}

void SkeletalModel::loadSkeleton( const char* filename )
{
	// Load the skeleton from file here.
	char buffer[MAX_BUFFER];
        string buffer2; 

	int depth;
	float pos[3]; 

	ifstream bob(filename); 
	if(!bob.is_open())
	{	
		cout << "file not open" <<endl;
		return; 
	}

	//use instead of !bob.eof(), causes us to read last line twice 

	//returns false at end of file
	while(bob.getline(buffer,MAX_BUFFER)) //now we need to extract data from each line and construct tree
	{	
		stringstream ss(buffer);
		ss >> pos[0] >> pos[1] >> pos[2] >> depth;
		cout << pos[0] << " " << pos[1] << " " << pos[2] << " " << depth <<endl;

		Joint * current=new Joint; 
		//define transform matrix associated with specific Joint
		(*current).transform=Matrix4f(1,0,0,pos[0],
					      0,1,0,pos[1],
					      0,0,1,pos[2],
				 	         0,0,0,1);

		m_joints.push_back(current); 
		if(depth==-1)
		{
			m_rootJoint=current;
		}
		else
		{
			((*m_joints[depth]).children).push_back(current); 
		}
	}

	bob.close();

	//cout << "Tree Size " << displayTree(m_rootJoint) << endl; //use this method to validate tree structure 
}

//this function calls helper on childen, and plots circle at current joint 
void SkeletalModel::helperJoint(Joint * bob)
{
	m_matrixStack.push((*bob).transform);//transform to frame of joint, place at origin	

	/*
	m_matrixStack.push(Matrix4f(1.0,0,0,0.0,
				    0,1.0,0,0.0,
				    0,0,1.0,0.0,
				    0,0,0,1.0  )); //translate box to appropriate location
	*/

	//cout << "Drawing joints " <<endl; 
	glLoadMatrixf( m_matrixStack.top());
	//glLoadIdentity();
	//m_matrixStack.top(); //print matrix stack we are using for each step, verify it makes sense 
	glutSolidSphere( 0.025f, 12, 12 ); //draw sphere in frame of current joint


	for(int n=0; n<((*bob).children).size(); n++)
		helperJoint(((*bob).children)[n]);  //will return transformed to this frame 

	m_matrixStack.pop();//pop off transformation and go to previous frame	

	return; 
}

void SkeletalModel::drawJoints( )
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.

	helperJoint(m_rootJoint);  //call helper on rootJoint, will recurse through entire tree 
	
}

//this function calls helper on childen, and plots circle at current joint 
void SkeletalModel::helperBones(Joint * bob)
{
	//first thing we need to do is calculate displacement vector, need to be careful not to preform any 
	//transformations on matrices before because will mess up

	Matrix4f d1=(*bob).transform; 
	
	Vector3f disp(d1(0,3),d1(1,3),d1(2,3));  //displacement vector between joints
	
	//cout << "length of displacement vector " << disp.abs() << endl; 

	//no addition or subtraction operator defined for matrices, annoying that doesnt give error 
	//Matrix4f diff=m_matrixStack.top()-(*bob).transform; //get diff matrix for determining vector between joints
	
	//m_matrixStack.push((*bob).transform);//transform to frame of joint, place at origin	

	//compute transformation matrix for rotating box to point to other joint

	//annoying for cross operations, need T.cross(arg1,arg2), T could have nothing to do with cross product 

	Vector3f T=disp.normalized(); 
	Vector3f N=(T.cross(Vector3f(0.1,0.2,0.5),T)).normalized(); //find 
	Vector3f B=(N.cross(N,T)).normalized(); 

	m_matrixStack.push(Matrix4f(N[0],B[0],T[0],0.0,
				    N[1],B[1],T[1],0.0,
				    N[2],B[2],T[2],0.0,
				    0,0,0,1.0  )); //rotate box

	m_matrixStack.push(Matrix4f(0.05,0,0,0.0,
				    0,0.05,0,0.0,
				    0,0,disp.abs(),0.0,
				    0,0,0,1.0  )); //scale box to be appropriate size 

	//now we need to push on transformations 
	m_matrixStack.push(Matrix4f(1.0,0,0,0.0,
				    0,1.0,0,0.0,
				    0,0,1.0,0.5,
				    0,0,0,1.0  )); //translate box to appropriate location

	glLoadMatrixf( m_matrixStack.top());  //apply transformations for cube
	glutSolidCube( 1.0f); //draw cube with transformations applied

	//cout << "Drawing bones " << endl; 

	//need to pop off 3 transformations associated with drawing box
	m_matrixStack.pop();	
	m_matrixStack.pop();
	m_matrixStack.pop();

	m_matrixStack.push((*bob).transform);//transform to frame of joint, place at origin	

	for(int n=0; n<((*bob).children).size(); n++)
		helperBones(((*bob).children)[n]);  //will return transformed to this frame 

	m_matrixStack.pop();//pop off transformation and go to previous frame	
	return; 
}


void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy.

	m_matrixStack.push((*m_rootJoint).transform);//transform to frame of joint, place at origin	
	
	//Don't call for root joint itself, only call for children
	for(int n=0; n<((*m_rootJoint).children).size(); n++)
		helperBones((*m_rootJoint).children[n]);  //call helper on rootJoint, will recurse through entire tree

	m_matrixStack.pop();	
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.

	//cout << "set transform " <<endl; 


	Matrix4f O=(*m_joints[jointIndex]).transform;
	//printMat(O);

	Matrix3f rotate1(cos(rX),sin(rX),0, 
		       -sin(rX),cos(rX),0,
			0,       0,     1);

	Matrix3f rotate2(1,      0,     0,
		         0   ,cos(rY),sin(rY),
			0,   -sin(rY),cos(rY));

	Matrix3f rotate3(cos(rZ),sin(rZ),0, 
		       -sin(rZ),cos(rZ),0,
			0,       0,     1);

	Matrix3f R=rotate1*rotate2*rotate3; 

	(*m_joints[jointIndex]).transform=Matrix4f(R(0,0),R(0,1),R(0,2),O(0,3), 
						   R(1,0),R(1,1),R(1,2),O(1,3), 
						   R(2,0),R(2,1),R(2,2),O(2,3), 
						   O(3,0),O(3,1),O(3,2),O(3,3)); 


	//O=(*m_joints[jointIndex]).transform;
	//printMat(O);

}

//compute inverse of Bind matrix for current node and continue traversing down
void SkeletalModel::helperBind(Joint * bob, Matrix4f Binv)
{
	Binv=((*bob).transform).inverse()*Binv; //update Bind inverse matrix for current node
	(*bob).bindWorldToJointTransform=Binv; 

	for(int n=0; n<((*bob).children).size(); n++)
		helperBind(((*bob).children)[n],Binv);  //will return transformed to this frame 
	
	return; 
}

void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.

	Matrix4f Binv=Matrix4f(1.0,0,0,0.0,
			       0,1.0,0,0.0,
			       0,0,1.0,0.0,
			       0,0,0,1.0  ); //translate box to appropriate location

	helperBind(m_rootJoint,Binv);
}

//compute transform matrix for each Joint
void SkeletalModel::helperCurrent(Joint * bob, Matrix4f T)
{
	T=T*(*bob).transform; //update Bind inverse matrix for current node
	(*bob).currentJointToWorldTransform=T; 

	for(int n=0; n<((*bob).children).size(); n++)
		helperCurrent(((*bob).children)[n],T);  //will return transformed to this frame 
	
	return; 
}

//void SkeletalModel::updateCurrentJointToWorldTransforms()

void SkeletalModel::updateCurrentJointToWorldTransforms()
{

	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's currentWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.

	Matrix4f T=Matrix4f(1.0,0,0,0.0,
			    0,1.0,0,0.0,
			    0,0,1.0,0.0,
			    0,0,0,1.0  ); //translate box to appropriate location

	helperCurrent(m_rootJoint,T);
}

void SkeletalModel::updateMesh()
{
	//cout << "updating mesh " <<endl; 

	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.

	//need to go through every mesh and update 

	for(int n=0; n<(m_mesh.bindVertices).size();n++)
	{
		Vector3f v=(m_mesh.bindVertices)[n]; //get vertex
		Vector4f initial(v[0],v[1],v[2],1); //put in proper format for multiplication by matrices
		
		Vector4f total(0.0,0.0,0.0,0.0); //this will accumulate sum of all transformations
		Vector4f a; 

		//cout << n  << " " << (m_mesh.bindVertices).size() <<endl;

		for(int n2=0; n2<((m_mesh.attachments)[n]).size();n2++)
		{
			Joint b=*(m_joints[n2+1]);  //root node always has 0 weight, number of attachments is joints-1
			a=(b.currentJointToWorldTransform)*(b.bindWorldToJointTransform)*initial;
			
			//cout <<a[0]<<" "<<a[1]<<" "<<a[2]<<" "<<a[3] <<endl; 

			/*
			if(n==101)
				printMat(b.bindWorldToJointTransform); //lets visualize these matrices

			if(n==100)
				cout <<a[0]<<" "<<a[1]<<" "<<a[2]<<" "<<a[3] <<endl; 
			*/	 

			//cout << (m_mesh.attachments[n])[n2] << " "; 

			total=total+(m_mesh.attachments[n])[n2]*a; //accumulate effects of all joints on vertex

			//cout << n2; 
		}
		//cout << endl; 

			//just try out with giving all the weight to a single joint
			//Joint b=*(m_joints[0]);
			//a=(b.currentJointToWorldTransform)*(b.bindWorldToJointTransform)*initial; 
			//total=total+1*a; //accumulate effects of all joints on vertex


		(m_mesh.currentVertices)[n]=Vector3f(total[0],total[1],total[2]); //update current vertices 
	}

	//cout << "finished updating mesh " <<endl; 

}

