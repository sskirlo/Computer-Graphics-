#include "Noise.h"
#include "PerlinNoise.h"

Noise::Noise() :
    octaves(0),
    init(false)
{
}

Noise::Noise(int _octaves,
             const Vector3f &color1, 
             const Vector3f &color2,
             float freq,
             float amp) :
    octaves(_octaves),
    frequency(freq),
    amplitude(amp)
{
    color[0] = color1;
    color[1] = color2;
    init = true;
}

Vector3f
Noise::getColor(const Vector3f &pos)
{
    // IMPLEMENT: Fill in this function ONLY.
    // Interpolate between two colors by weighted average

	float val=sin(frequency*pos[0]+amplitude*perlinOctaveNoise(pos,octaves)); 
	
	float clamp=0.4; 
	
	if(val<-(1-clamp)) //clamp between clamp
		val=-(1-clamp); 
	if(val>(1-clamp))
		val=(1-clamp); 
	val=val/2/(1-clamp); //rescale -0.5,0.5
	val=val+0.5; 
		
	Vector3f colornew=val*color[0]+(1-val)*color[1]; 

    return colornew;
}

bool
Noise::isValid()
{
    return init;
}
