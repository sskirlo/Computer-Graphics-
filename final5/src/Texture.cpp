#include "Texture.h"
#include <iostream>

using namespace std; 

Texture::Texture():
    _image(NULL)
{
}

Texture::~Texture()
{
    delete _image; _image = NULL;
}

void
Texture::load(const std::string &filename)
{
    delete _image;
    _image = Image::loadPNG(filename);
	cout << "loaded png " << filename <<endl;
    assert(_image);
}

Vector3f
Texture::getTexel(float x, float y) const
{
	//cout << x << " " << y <<endl;
	if(_image==NULL)	
		cout << "image is null" <<endl;
	//cout << "w, h" << _image->getWidth() << _image->getHeight()<<endl; 

    x = x * _image->getWidth();
    y = (1 - y) * _image->getHeight();
    int ix = (int) x;
    int iy = (int) y;
    float alpha = x - ix;
    float beta = y - iy;

    const Vector3f &pixel0 = getPixel(ix + 0, iy + 0);
    const Vector3f &pixel1 = getPixel(ix + 1, iy + 0);
    const Vector3f &pixel2 = getPixel(ix + 0, iy + 1);
    const Vector3f &pixel3 = getPixel(ix + 1, iy + 1);

    Vector3f color;
    for (int ii = 0; ii < 3; ii++) {
        color[ii] = 
              (1 - alpha) * (1 - beta) * pixel0[ii]
            +      alpha  * (1 - beta) * pixel1[ii]
            + (1 - alpha) *      beta  * pixel2[ii]
            +      alpha  *      beta  * pixel3[ii];
    }

    return color;
}
