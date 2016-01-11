#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"

#include <vecmath.h>
#include <float.h>
#include <cmath>

class Camera
{
  public:
    virtual ~Camera() {}

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0; 

    virtual float getTMin() const = 0; 
};

/// TODO: Implement Perspective camera
/// Fill in functions and add more fields if necessary
class PerspectiveCamera : public Camera
{
  public:
    PerspectiveCamera(const Vector3f &center, 
                      const Vector3f &direction,
                      const Vector3f &up, 
                      float angle) :
        _center(center),
        _direction(direction.normalized()),
        _up(up),
        _angle(angle)
    {
        _horizontal=(_direction.cross(_direction,_up)).normalized(); 
    }

	//for now we do not take into account aspect ratio of y, set to 1, may cause distortion
    virtual Ray generateRay(const Vector2f &point)
    {
        // calculate the ray origin and direction based on the pixel we want to intersect

	float D=1/tan(_angle/2.0); 
        
	Vector3f direction=point[0]*_horizontal+point[1]*_up+D*_direction;
	return Ray(_center, direction);  
    }

    virtual float getTMin() const 
    { 
        return 0.0f;
    }

  private:
    Vector3f _center; 
    Vector3f _direction;
    Vector3f _up;
    float _angle;
    Vector3f _horizontal;
};

#endif //CAMERA_H
