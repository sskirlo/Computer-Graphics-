#include "Renderer.h"

#include "Image.h"
#include "Camera.h"

using namespace std; 

Renderer::Renderer(const std::string &sceneFilename)
{
    // TODO: implement

	//just save Scene filename 
	fname=sceneFilename; 

}

Renderer::~Renderer() //why do we need this? 
{
    // TODO: free up resources
}

//we moved this to the main loop, couldn't get to compile properly within material class, also easier for debugging 

Vector3f shade(const Ray &ray, const Hit &hit, SceneParser * Scene )
{
	Material * material=hit.getMaterial();

	//need to get a specific diffuse color depending on whether or not we have triangle object
	//and whether or not this triangle object contains a texture 

	//as default set as diffuse color
	Vector3f _diffuseColor=material->getDiffuseColor();

	Vector2f uv; 
	Texture * tex; 

	if(hit.hasTex())
	{
		if((*material).isLoaded())
			cout << "texture should have been loaded" <<endl; 
		else
			cout << "Material is not loaded!" <<endl;

		uv=hit.getTexCoord();
		tex=(*material).getTexture();
		cout << "texture coordinates" << uv[0] << " " << uv[1] <<endl; 
		_diffuseColor=(*tex).getTexel(uv[0],uv[1]); //set diffuse color based on texture map
	}

	Vector3f _specularColor=material->getSpecularColor();
	float _shininess=material->getShininess(); 

        //first we need to figure out where this section of material is located
        Vector3f n=hit.getNormal();
        Vector3f dray=ray.getDirection();
        Vector3f p=ray.pointAtParameter(hit.getT());
        Vector3f c_amb=Scene->getAmbientLight();
        int num=Scene->getNumLights();

        Vector3f color=_diffuseColor*c_amb; //* is direct vector product

        Light * bob;
        Vector3f ldir, col,ref;
        float dist,val,val2;

        //iterate through light sources
        for(int i=0; i<num; i++)
        {
                bob=Scene->getLight(i);
                bob->getIllumination(p,ldir,col,dist);

                //compute contribution from diffuse reflection
                val=ldir.dot(ldir,n);
                if(val<0) //clamp value
                        val=0;
                color=color+_diffuseColor*col*val;

                //compute contribution from specular reflection
                ref=ldir-2*ldir.dot(ldir,n)*n;
                val2=pow(dray.dot(dray,ref),_shininess); //as dray moves off reflection we suppress 
                if(dray.dot(dray,ref)<0) //clamp value
                        val2=0;

                color=color+_specularColor*col*val2;
        }

        return color;
}


void
Renderer::Render(int width,
                 int height,
                 const std::string &outputFilename,
                 float minDepth,
                 float maxDepth,
                 const std::string &depthFilename,
                 const std::string &normalFilename)
{
    // TODO: implement

    // Loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene. Write the color at the intersection to that
    // pixel in your output image.
    //
    // Use the Image class to write out the requested output files.

	SceneParser Scene(fname); //create scene here 

	//scene is properly being read in
	cout << "Number of materials in scene " << Scene.getNumMaterials() << endl; 

	//just initialize these all anyways, very little memory not to, and annoying to deal with conditional
	//initialization
	Image IM=Image(width,height); 
	Image ID=Image(width,height); 
	Image IN=Image(width,height); 

	bool depth=false;
	if(!(depthFilename.empty()))  //we will produce depth image
		depth=true; 

	bool normal=false;
	if(!(normalFilename.empty()))  //we will produce normal image
		normal=true; 

	float xp;
	float yp; 
	Ray R;
	bool hitSomething; 
	Vector3f color; 
	float T; 

	for(int x=0; x<width; x++ )
	{
		for(int y=0; y<height; y++)
		{
			Hit H;  //need to reinitialize every single time 
	                        //unless we want it to remmeber things like the texture state 

			H.set(std::numeric_limits<float>::max(),NULL,Vector3f(0,0,0)); //initialization for Hit

			//cout << "pixel " << x<< " " <<y <<endl; 

			//compute coordinates for ray we want to generate 
			xp=2*((float)x)/((float)(width-1))-1; 			
			yp=2*((float)y)/((float)(height-1))-1; 		
			//generate ray object for camera 
			R=(Scene.getCamera())->generateRay(Vector2f(xp,yp));			

			//cout << "generated ray " <<endl;

			Group * gro=Scene.getGroup();

			//iterate through group objects and see which is closest hit for ray
			hitSomething=gro->intersect(R,(Scene.getCamera())->getTMin(),H);

			//cout << " hit " <<hitSomething <<endl;
			
			//set color of pixels for images based on what we get back for the ray
			if(hitSomething==false)  //set pixel to be background color, didn't hit anything
			{
				//cout << "setbackground " <<endl;
				IM.setPixel(x,y,Scene.getBackgroundColor());
				if(depth)
					ID.setPixel(x,y,Vector3f(0,0,0)); 	
				if(normal)
					IN.setPixel(x,y,Vector3f(0,0,0));

				//cout << "finished set background"<<endl;  	
			}
			else
			{
				//cout << "set Material "<<endl; 
				IM.setPixel(x,y,shade(R,H,&Scene)); //shade method is defined above 
				//IM.setPixel(x,y,Vector3f(0.5,0.5,0.5));
				if(depth)  //calculate Pixel color based on depth, set to gray scale 
				{
					T=H.getT();
					if(T<minDepth)
						T=minDepth; 
					if(T>maxDepth)
						T=maxDepth;
					color=Vector3f(1,1,1)-((T-minDepth)/(maxDepth-minDepth)*Vector3f(1,1,1)); 					
					ID.setPixel(x,y,color);
				}
				if(normal)
				{
					color=H.getNormal();
					color=Vector3f(fabs(color[0]),fabs(color[1]),fabs(color[2])); 					
					IN.setPixel(x,y,color);
				}
				//cout << "finished set material " <<endl; 
			}
		}
	}

	//save the images 

	IM.savePNG(outputFilename); 

	if(depth)  //we will produce depth image
		ID.savePNG(depthFilename); 

	if(normal)  //we will produce depth image
		IN.savePNG(normalFilename); 
	

}
