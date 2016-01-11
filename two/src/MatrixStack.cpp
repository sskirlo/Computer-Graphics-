#include "MatrixStack.h"
#include <iostream>


std::vector< Matrix4f > m_matrices;



MatrixStack::MatrixStack()
{
	Matrix4f iden(1.0,0,0,0,
                      0,1.0,0,0,
                      0,0,1.0,0,
                      0,0,0,1.0);
	m_matrices.push_back(iden);
	// Initialize the matrix stack with the identity matrix.
}

void printMat1(Matrix4f bob)
{
	std::cout << "push " << std::endl;
	for(int n1=0;n1<4; n1++)
	{
		for(int n2=0; n2<4; n2++)
		{
			std::cout<< bob(n1,n2) <<" ";
		}
		std::cout << std::endl; 
	}
	std::cout << std::endl; 
}

void MatrixStack::printStack()
{
	std::cout << "print stack" << std::endl;
	for(int n=0; n<m_matrices.size(); n++)
		printMat1(m_matrices[n]);	
	std::cout << "printed stack " << m_matrices.size() << std::endl;	
}

void MatrixStack::clear()
{
	//std::cout << "clearing" << std::endl;
	
	while(m_matrices.size()>1)
	{
		m_matrices.pop_back(); 
	}
	// Revert to just containing the identity matrix.
}

Matrix4f MatrixStack::top()
{
	//for debugging purposes we also want to printStack
	//printStack();

	return m_matrices.back();
}

void MatrixStack::push( const Matrix4f& m )
{
	// Push m onto the stack.
	// Your stack should have OpenGL semantics:
	// the new top should be the old top multiplied by m
	//Matrix4f bob=new Matrix4f(m_matrices.back()*m);
	Matrix4f bob=m_matrices.back()*m;
	
	m_matrices.push_back(bob);

	//m_matrices.push_back(*(new Matrix4f(bob)));
	//printMat1(m_matrices.back()); 
	//std::cout << "current stack size " << m_matrices.size()  << std::endl;
	//printStack();

	//glLoadMatrixf(m_matrices.front());  
}

void MatrixStack::pop()
{
	//std::cout << "pop" <<std::endl; 	

	// Remove the top element from the stack
	if(m_matrices.size()>1)
		m_matrices.pop_back();
	else
		std::cout << "will not pop identity matrix, push and pops unbalanced " << std::endl; 
	//glLoadMatrixf(m_matrices.front());  
}
