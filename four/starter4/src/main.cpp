#include <cstring>
#include <iostream>

#include "Renderer.h"

int main(int argc, const char **argv)
{
    // Report help usage if no args specified.
    if (argc == 1) {
        std::cout << "Usage: a4 "
            << "-input <scene> -size <width> <height> -output <image.png> -depth <depth_min> <depth_max> <depth_image.png> [-normals <normals_image.png>]\n";
        return 1;
    }

    std::string sceneFilename;
    int width = 0;
    int height = 0;
    std::string outputFilename;
    std::string depthFilename;
    float minDepth = 0.0f;
    float maxDepth = 0.0f;
    std::string normalFilename;

    // Fill in your implementation here.
    // Parse arguments.

    int argNum=1; 
    while(argNum<argc) {
        //std::cout 
        //    << "Argument " << argNum 
        //    << " is: " << argv[argNum] << std::endl;

	if("-input"==std::string(argv[argNum]))
	{
		argNum++;			
		sceneFilename=std::string(argv[argNum]);
		std::cout << "SceneFilename " <<sceneFilename << std::endl;  		
	}
	else if("-output"==std::string(argv[argNum]))
	{
		argNum++;			
		outputFilename=std::string(argv[argNum]); 	
		std::cout << "OutputFilename " <<outputFilename << std::endl;  		
	}		
	else if("-normal"==std::string(argv[argNum]))
	{
		argNum++;			
		normalFilename=std::string(argv[argNum]); 			
		std::cout << "normalFilename " <<normalFilename << std::endl;  		
	}
	else if("-size"==std::string(argv[argNum]))
	{	
		argNum++;
		width=atoi(argv[argNum]); //convert  
		argNum++;
		height=atoi(argv[argNum]);
		std::cout << "size " << width <<" " << height << std::endl;  		
	} 
	else if("-depth"==std::string(argv[argNum]))
	{
		argNum++;
		minDepth=atof(argv[argNum]); 
		argNum++;
		maxDepth=atof(argv[argNum]);
		argNum++;
		depthFilename=std::string(argv[argNum]);
		std::cout << "depth " <<minDepth << " " << maxDepth << depthFilename << std::endl;  		
	} 				
	else
	{
		std::cout << "invalid " << argv[argNum] << std::endl; 
	}
	argNum++; 
    }

    Renderer renderer(sceneFilename);
    renderer.Render(width, height, 
                    outputFilename,
                    minDepth, maxDepth,
                    depthFilename, 
                    normalFilename);

    return 0;
}
