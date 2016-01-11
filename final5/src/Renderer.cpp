#include "Renderer.h"

#include "ArgParser.h"
#include "Camera.h"
#include "Hit.h"
#include "Image.h"
#include "Ray.h"
#include "VecUtils.h"

#include <limits>

#define EPSILON 0.001 //epsilon for offsets

Renderer::Renderer(const ArgParser &args) :
    _args(args),
    _scene(args.input_file)
{
}

Renderer::~Renderer()
{
}



//names of variables in argparser 

/*
// rendering output
std::string input_file;
std::string output_file;
std::string depth_file;
std::string normals_file;
int width;
int height;
int stats;

// rendering options
float depth_min;
float depth_max;
int bounces;
bool shadows;

// supersampling
bool jitter;
bool filter;
*/

void
Renderer::Render()
{
    // TODO: IMPLEMENT 

    // This is the main loop that should be based on your assignment 4 main
    // loop. You will need to modify it to switch from ray casting to ray
    // tracing, to cast shadows and anti-aliasing (via jittering and
    // filtering).

	//scene is properly being read in
	//cout << "Number of materials in scene " << _scene.getNumMaterials() << endl; 

	int sc=1; 

	if(_args.jitter) //jitter is true
	{
		sc=3; 
	}

	//just initialize these all anyways, very little memory not to, and annoying to deal with conditional
	//initialization
	Image IM=Image(_args.width*sc,_args.height*sc); 
	Image ID=Image(_args.width*sc,_args.height*sc); 
	Image IN=Image(_args.width*sc,_args.height*sc); 

	bool depth=false;
	if(!((_args.depth_file).empty()))  //we will produce depth image
		depth=true; 

	bool normal=false;
	if(!((_args.normals_file).empty()))  //we will produce normal image
		normal=true; 

	float xp;
	float yp; 
	Ray R;
	bool hitSomething; 
	Vector3f color; 
	float T; 
	float Tmin=1000000;
	float Tmax=-100000; 

	for(int x=0; x<(_args.width*sc); x++ )
	{
		
		if((x%30)==0)
			cout << x <<endl; 
		
		for(int y=0; y<(_args.height*sc); y++)
		{
			
			Hit H;  //need to reinitialize every single time 
	                        //unless we want it to remmeber things like the texture state 
			H.set(std::numeric_limits<float>::max(),NULL,Vector3f(0,0,0)); //initialization for Hit

			//cout << endl; 
			//cout << "pixel " << x<< " " <<y <<endl; 

			float rx=0;
			float ry=0; 
			if(_args.jitter)
			{
				rx=(static_cast <float> (rand()) / static_cast <float> (RAND_MAX))-0.5;;
				ry=(static_cast <float> (rand()) / static_cast <float> (RAND_MAX))-0.5;
			}

			//compute coordinates for ray we want to generate 
			xp=2*(((float)x)+rx)/((float)(sc*_args.width-1))-1; 			
			yp=2*(((float)y)+ry)/((float)(sc*_args.height-1))-1; 		
			//generate ray object for camera 
			R=(_scene.getCamera())->generateRay(Vector2f(xp,yp));	//camera ray normalized 		

			color=traceRay(R,EPSILON, _args.bounces, 1, H); //1=refractive index as a start 
			//need Hit H to remmeber details of first object hit, not recursed one, need to be careful
				
			IM.setPixel(x,y,color); //shade method is defined above 
			
			/*
			int valx=273; 
			int valy=234;
			int r=2; //range we want to set
			
			//we want to id specific pixel which is causing a problem 
			if(x>(valx-r) && x<(valx+r) && y>(valy-r) && y<(valy+r) )
				IM.setPixel(x,y,Vector3f(0,0,0)); 
			*/
			
			//print min, max T at end for reference
			
			//IM.setPixel(x,y,Vector3f(0.5,0.5,0.5));
			if(H.getMaterial()==NULL)  //didn't register a hit on anything
			{ 
				if(depth)
					ID.setPixel(x,y,Vector3f(0,0,0)); 	
				if(normal)
					IN.setPixel(x,y,Vector3f(0,0,0));
			}			
			else
			{
				float Tcomp=H.getT();
				if(Tcomp>Tmax)
					Tmax=Tcomp;
				if(Tcomp<Tmin)
					Tmin=Tcomp; 
				
				if(depth)  //calculate Pixel color based on depth, set to gray scale 
				{
					T=H.getT();
					if(T<_args.depth_min)
						T=_args.depth_min; 
					if(T>_args.depth_max)
						T=_args.depth_max;
					color=Vector3f(1,1,1)-((T-_args.depth_min)/(_args.depth_max-_args.depth_min)*Vector3f(1,1,1)); 					
					ID.setPixel(x,y,color);
				}
				if(normal)
				{
					color=H.getNormal();
					color=Vector3f(fabs(color[0]),fabs(color[1]),fabs(color[2])); 					
					IN.setPixel(x,y,color);
				}
			}
				//cout << "finished set material " <<endl; 
		}
	}

	//save the images 

	cout << "tmin " << Tmin << "Tmax " << Tmax << endl;

	if(!_args.filter)
		IM.savePNG(_args.output_file); 

	if(depth)  //we will produce depth image
		ID.savePNG(_args.depth_file); 

	if(normal)  //we will produce depth image
		IN.savePNG(_args.normals_file); 
		
	//we only downsample and filter if we have already jittered
	if(_args.jitter && _args.filter)
	{
		cout << "assuming image size divisible by 3" <<endl; 
		Image IFilter=Image(_args.width*sc,_args.height*sc); //create images for downsampling and filtering
		Image IDown=Image(_args.width*sc/3,_args.height*sc/3);  
		
		float k []={0.1201,0.2339,0.2931,0.2339,0.1201}; 
		
		for(int x=0; x<_args.width*sc; x++)
		{
			for(int y=0; y<_args.height*sc; y++)
			{
				IFilter.setPixel(x,y,Vector3f(0,0,0)); 
				Vector3f color(0,0,0);
				for(int x1=-2; x1<3; x1++)
				{
					int coord=x1+x; 
					
					//handle edge conditions 
					if(coord<0)
						coord=0;
					if(coord>(_args.width*sc-1))
						coord=(_args.width*sc-1); 
					
					color=IM.getPixel(coord,y)*k[2+x1]+color; 
				}
				IFilter.setPixel(x,y,color); 
			}
		}
		
		IFilter.savePNG("Filtered.png"); //save Filtered file separately for debugging 
		//downsample
		for(int x=0; x<_args.width*sc/3; x++)
		{
			for(int y=0; y<_args.height*sc/3; y++)
			{
				Vector3f color(0,0,0);
				for(int x1=0; x1<3; x1++)
				{
					for(int y1=0; y1<3; y1++)
					{
						color=color+IFilter.getPixel(3*x+x1,3*y+y1);
					}
				}
				
				IDown.setPixel(x,y,color/9.0); //average 3,3 pixels in cell 
			}
		}
		IDown.savePNG(_args.output_file); 
	}
	

}

// These function definitions are mere suggestions. Change them as you like.
Vector3f mirrorDirection(const Vector3f &normal, const Vector3f &incoming)
{
    // TODO: IMPLEMENT

    //need to watch conventions, we are assuming with this equation that normal and incoming have certain relationship
    //need to make sure flipping normal or incoming direction doesn't change things, should be okay here 

	Vector3f ref=incoming-2*incoming.dot(normal,incoming)*normal;
	//ref.normalize();

    return ref;
}

bool transmittedDirection(const Vector3f &normal, const Vector3f &incoming, float index_i,float index_t, Vector3f &transmitted)
{
	//we have to check if we have total internal reflection 
	float nr=index_i/index_t; 
	//cout << "index t index i, normal " <<index_t << " "<<index_i <<endl;  
	//normal.print(); 
	
	float nid=normal.dot(normal,incoming);  //need to watch convention for normal and incident ray direction 
	
	//need to fix conventions for the normal to be pointing towards the incoming reflective ray, otherwise the 
	//equation will not properly compute the refracted ray 
	Vector3f norm=normal; 
	
	if(nid>0)
	{
		nid=-nid; 
		norm=-normal; 
	}
	
	float val=(1-pow(nr,2.0)*(1-pow(nid,2.0)));
	//cout << val << endl; 
	if(val<0)
	{
		//cout << "total internal reflection! " <<endl; 
		return false;  //we have total internal reflection
	}
	else
	{
		transmitted=nr*incoming-((nr*nid+pow(val,0.5)))*norm;
		transmitted.normalize(); 
		
		//if index equals 1, incoming ray should equal the outgoing ray, otherwise something is wrong 
		//cout << "index " << nr << endl;
		//incoming.print();
		//transmitted.print();
		
		return true; 
	}
    // TODO: IMPLEMENT
    return false;  //make compiler happy 
}

Vector3f Renderer::helperShade(const Ray &ray, Hit &hit,float tmin) const 
{
	//Vector3f _specularColor=material->getSpecularColor();
	//float _shininess=material->getShininess(); 

    //first we need to figure out where this section of material is located
    Material * material=hit.getMaterial();
    Vector3f n=hit.getNormal();
	Vector3f dray=(ray.getDirection()).normalized(); //need to watch out ray no longer necessarily normalized because of transform class
    Vector3f p=ray.pointAtParameter(hit.getT());
    Vector3f c_amb=_scene.getAmbientLight();
    int num=_scene.getNumLights();

	//calculate diffuse color
    Vector3f color=material->getDiffuseColor()*c_amb; //* is direct vector product
	//Vector3f color(0,0,0); //ignore diffuse lighting for now, seems like we need to call Material class

    Light * bob;
    Vector3f ldir, col,ref;
    float dist,val,val2;

    //iterate through light sources
    for(int i=0; i<num; i++)
    {
    	bob=_scene.getLight(i);
        bob->getIllumination(p,ldir,col,dist);
		if(!(_args.shadows)) //do not cast rays for shadows
		{
        	color=color+(hit.getMaterial())->shade(ray,hit,ldir,col);
		}
		else  //do shadow casting
		{
			Ray ray2(p,ldir.normalized()); //point and direction to light
			Hit hit2(dist,NULL,Vector3f(0,0,0)); //initialize with distance to lightsource, if after intersection still this, we know no hits
			_scene.getGroup()->intersect(ray2,tmin,hit2);
			if(hit2.getT()==dist) //didn't hit anything, free shot to light source 
			{
				color=color+(hit.getMaterial())->shade(ray,hit,ldir,col); 
			}
		}
    }

    return color;
}

//0 bounces means we still cast 1 more ray 
Vector3f Renderer::traceRay(const Ray &ray, float tmin, int bounces, float refr_index, Hit &hit) const
{
	//cout << "recursion level " << bounces <<endl;
	//cout << endl; 
	
	//get all objects and check intersection
	Group * gro=_scene.getGroup();

	//iterate through group objects and see which is closest hit for ray
	bool hitSomething=gro->intersect(ray,tmin,hit);

	//cout << " hit " <<hitSomething <<endl;
	
	if(bounces==-1)
	{
		return Vector3f(0,0,0);  //return nothing if we have recursed too far down, DONT return background, otherwise this is different 
		                         //than the sample code, 
	}
	
	if(hitSomething==false)  //set pixel to be background color, didn't hit anything, or if we reached max recursion depth
	{
		//if(bounces==0) //these will be rays which are reflected off box surface 
		//{
			//cout << "get from background " <<endl; 
		//}
		return (_scene.getBackgroundColor(ray.getDirection())); 	
	}
	else
	{
		//print recursion and hit distance
		//cout << "recursion level and hit T " << bounces <<" " << hit.getT() << endl; 
		
		
		Vector3f color(0,0,0);
		color=helperShade(ray,hit,tmin)+color; //calculate Shadow Rays
		
		Material * mat=hit.getMaterial();
		
		Vector3f colorrefl(0,0,0);
		Vector3f colortrans(0,0,0); 
		Vector3f refl_dir;
		Vector3f trans_dir; 
		
		Vector3f n=hit.getNormal();
		Vector3f dray=(ray.getDirection()).normalized();
	    Vector3f p=ray.pointAtParameter(hit.getT());
		
		//compute reflected rays if object is reflective
		//if(mat->getSpecularColor()!=Vector3f(0,0,0)) //has a specular color so surface is reflective
		if(true) //all objects have specular reflection? 
		{
			//cout << "created reflected ray bounces " << bounces <<endl; 
			
			Hit hitn(std::numeric_limits<float>::max(), NULL, Vector3f(0, 0, 0));
			//calculate reflected ray
			refl_dir=mirrorDirection(n,dray);
			Ray rayn(p,refl_dir); 
			colorrefl=traceRay(rayn,tmin,bounces-1,refr_index,hitn)*mat->getSpecularColor(); //reflected ray keeps index of medium currently in
			//save reflected color for later after we have transmitted color, need to handle case carefully
			
			//cout << "end reflected ray trace color bounces " << bounces <<endl; 
			//colorrefl.print(); 
			
			//print out data about reflected ray
			//cout << endl; 
			/*if(bounces==1)
			{
				colorrefl.print();
				refl_dir.print(); //we want to check if problem is from reflection direction  
				cout << "Hit T , Tmin " << hitn.getT() << " " << tmin <<endl; 
			}*/
		}
		
		bool trans=false;
		
		float next_index=mat->getRefractionIndex(); 
		
		//compute refracted rays, need to include reflection and transmission, if they are present
		if(next_index>0) //means we can refract through
		{
			Hit hitn(std::numeric_limits<float>::max(), NULL, Vector3f(0, 0, 0));
			//calculate transmitted ra
			
			//if refr_index>1, we assume we are LEAVING refractive media, so we set the next index as 1
			//because we ASSUME we are going into air, this in general is not valid, but this what we are
			//supposed to assume here 
			
			if(refr_index>1) //already in refractive media, intersection must mean LEAVING refractive media
			{
				next_index=1; //assume we are entering air 
			}
			
			trans=transmittedDirection(n,dray,refr_index,next_index,trans_dir);
			
			//lets print out information about the transmitted ray 
			//cout << "level and bool trans " << bounces << " " << trans <<endl; 
			//dray.print();
			//trans_dir.print();
			
			/*
			if(!trans && bounces==1)
			{
				cout << "total internal reflection, bounce 1" << endl; 
			}
			*/
			
			//need to handle case appropriately when we have total internal reflection 
			if(trans)
			{
				//cout << "create refracted ray level" << bounces <<endl;
				//dray.print();
				//trans_dir.print();
				
				Ray rayn(p,trans_dir.normalized());
				colortrans=traceRay(rayn,tmin,bounces-1,next_index,hitn); 
				
				//cout << "end transmitted ray trace color bounces " << bounces <<endl; 
				//colortrans.print();
			}
			//else don't cast new ray 
		}
		
		if(!trans)
		{
			color=color+colorrefl; //just add reflected color
		}
		else
		{
			//need to use Schlick approximation if we have transparent material without total internal reflection
			float R0=(next_index-refr_index)/(next_index+refr_index);
			R0=pow(R0,2.0); 
			float c=0;
			if(refr_index < mat->getRefractionIndex() )
			{
				c=abs(dray.dot(dray,n));
			}
			else
			{
				c=abs(trans_dir.dot(trans_dir,n)); //when we get total internal reflection, c->0, so R=1, all the power goes into the
				                                   //reflected light, this is taken into account above when !trans, because we just 
				                                   //directly add the reflected color, so we don't have to worry about 
				                                   //discontinuous behavior
			}
			float R=R0+(1-R0)*pow(1-c,5.0);  //c->0 with total internal reflection, and R=1 
			
			//color=colortrans; //lets just see transmission color for now 
			color=color+R*colorrefl+(1-R)*colortrans; //sum contributions from reflected and transmitted rays 
		}
		
		return color; 
 	}

    return Vector3f(0, 0, 0); //just to make compiler happy
}
