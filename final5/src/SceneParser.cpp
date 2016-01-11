#include <cstdio>
#include <cstring>
#include <cstdlib>
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "SceneParser.h"
#include "Camera.h" 
#include "Light.h"
#include "Material.h"

#include "Object3D.h"
#include "Group.h" 
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "Transform.h"

#define DegreesToRadians(x) ((M_PI * x) / 180.0f)

static 
void
_PostError(const std::string &msg)
{
    std::cout << msg;
    exit(1);
}

SceneParser::SceneParser(const std::string &filename) :
    _file(NULL),
    _camera(NULL),
    _background_color(0.5, 0.5, 0.5),
    _ambient_light(0, 0, 0),
    _num_lights(0),
    _num_materials(0),
    _current_material(NULL),
    _group(NULL),
    _cubemap(NULL)
{
    // parse the file
    assert(!filename.empty());

    if (filename.size() <= 4) {
        _PostError("ERROR: Wrong file name extension\n");
    }

    std::string ext = filename.substr(filename.size() - 4, 4);
    if (ext != ".txt") {
        _PostError("ERROR: Wrong file name extension\n");
    }

    _file = fopen(filename.c_str(), "r");

    if (_file == NULL) {
        _PostError(std::string("Cannot open scene file ") + filename + "\n");
    }

    parseFile();
    fclose(_file); 
    _file = NULL;

    // if no lights are specified, set ambient light to white
    // (do solid color ray casting)
    if (_num_lights == 0) {
        std::cerr << "WARNING: No lights specified\n";
        _ambient_light = Vector3f(1, 1, 1);
    }
}

SceneParser::~SceneParser() 
{
    delete _group;
    delete _camera;
    for (auto *material : _materials) {
        delete material;
    }
    for (auto *light : _lights) {
        delete light;
    }
    delete _cubemap;
}

// ====================================================================
// ====================================================================

void
SceneParser::parseFile() 
{
    //
    // at the top level, the scene can have a camera, 
    // background color and a group of objects
    // (we add lights and other things in future assignments)
    //
    char token[MAX_PARSER_TOKEN_LENGTH];        
    while (getToken(token)) { 
        if (!strcmp(token, "PerspectiveCamera")) {
            parsePerspectiveCamera();
        } else if (!strcmp(token, "Background")) {
            parseBackground();
        } else if (!strcmp(token, "Lights")) {
            parseLights();
        } else if (!strcmp(token, "Materials")) {
            parseMaterials();
        } else if (!strcmp(token, "Group")) {
            _group = parseGroup();
        } else {
            _PostError(
                std::string("Unknown token in parseFile: '") + token + "'\n");
            exit(1);
        }
    }
}

// ====================================================================
// ====================================================================

void
SceneParser::parsePerspectiveCamera() 
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    // read in the camera parameters
    getToken(token); assert(!strcmp(token, "{"));
    getToken(token); assert(!strcmp(token, "center"));
    Vector3f center = readVector3f();
    getToken(token); assert(!strcmp(token, "direction"));
    Vector3f direction = readVector3f();
    getToken(token); assert(!strcmp(token, "up"));
    Vector3f up = readVector3f();
    getToken(token); assert(!strcmp(token, "angle"));
    float angle_degrees = readFloat();
    float angle_radians = (float) DegreesToRadians(angle_degrees);
    getToken(token); assert(!strcmp(token, "}"));
    _camera = new PerspectiveCamera(center, direction, up, angle_radians);
}

void
SceneParser::parseBackground() 
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    // read in the background color
    getToken(token); assert(!strcmp(token, "{"));    
    while (true) {
        getToken(token); 
        if (!strcmp(token, "}")) { 
            break;    
        } else if (!strcmp(token, "color")) {
            _background_color = readVector3f();
        } else if (!strcmp(token, "ambientLight")) {
            _ambient_light = readVector3f();
        } else if (!strcmp(token, "cubeMap")) {
            _cubemap = parseCubeMap();
        } else {
            printf ("Unknown token in parseBackground: '%s'\n", token);
            assert(0);
        }
    }
}

CubeMap *
SceneParser::parseCubeMap()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token);
    return new CubeMap(token);
}

// ====================================================================
// ====================================================================

void
SceneParser::parseLights() 
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token); assert(!strcmp(token, "{"));
    // read in the number of objects
    getToken(token); assert(!strcmp(token, "numLights"));
    _num_lights = readInt();
    // read in the objects
    int count = 0;
    while (_num_lights > count) {
        getToken(token); 
        if (!strcmp(token, "DirectionalLight")) {
            _lights.push_back(parseDirectionalLight());
        } else if(strcmp(token, "PointLight")==0)
        {
            _lights.push_back(parsePointLight());
        }
        else {
            printf ("Unknown token in parseLight: '%s'\n", token); 
            exit(0);    
        }     
        count++;
    }
    getToken(token); assert(!strcmp(token, "}"));
}


Light *
SceneParser::parseDirectionalLight() 
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token); assert(!strcmp(token, "{"));
    getToken(token); assert(!strcmp(token, "direction"));
    Vector3f direction = readVector3f();
    getToken(token); assert(!strcmp(token, "color"));
    Vector3f color = readVector3f();
    getToken(token); assert(!strcmp(token, "}"));
    return new DirectionalLight(direction,color);
}

Light *
SceneParser::parsePointLight() 
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    Vector3f position, color;
    float falloff = 0;
    getToken(token); assert(!strcmp(token, "{"));
    while (true) {
        getToken(token); 
        if (!strcmp(token, "position")) {

            position = readVector3f();
        } else if (!strcmp(token, "color")) {
            color = readVector3f();
        } else if (!strcmp(token, "falloff")) {
            falloff = readFloat();
        } else {
            assert(!strcmp(token, "}"));
            break;
        }
    }
    return new PointLight(position, color, falloff);
}

// ====================================================================
// ====================================================================

void
SceneParser::parseMaterials() 
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token); assert(!strcmp(token, "{"));
    // read in the number of objects
    getToken(token); assert(!strcmp(token, "numMaterials"));
    _num_materials = readInt();
    // read in the objects
    int count = 0;
    while (_num_materials > count) {
        getToken(token); 
        if (!strcmp(token, "Material") ||
            !strcmp(token, "PhongMaterial")) {
            _materials.push_back(parseMaterial());
        } else {
            printf ("Unknown token in parseMaterial: '%s'\n", token); 
            exit(0);
        }
        count++;
    }
    getToken(token); assert(!strcmp(token, "}"));
}    

Material *
SceneParser::parseMaterial() 
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    filename[0] = 0;
    Vector3f diffuseColor(1,1,1), specularColor(0,0,0);
    float shininess = 0;
    float refractionIndex = 0;
    Noise *noise = NULL;
    getToken(token); assert(!strcmp(token, "{"));
    while (true) {
        getToken(token); 
        if (strcmp(token, "diffuseColor")==0) {
            diffuseColor = readVector3f();
        }
        else if (strcmp(token, "specularColor")==0) {
            specularColor = readVector3f();
        }
        else if (strcmp(token, "shininess")==0) {
            shininess = readFloat();
        } 
        else if(strcmp(token, "refractionIndex")==0){
            refractionIndex = readFloat();
        }
        else if (strcmp(token, "texture")==0) {
            getToken(filename);
        }
        else if (strcmp(token, "bump")==0) {
            getToken(token);
        }
        else if(strcmp(token,"Noise")==0){
            noise = parseNoise();
        }
        else {
            assert(!strcmp(token, "}"));
            break;
        }
    }
    Material *answer = new Material(diffuseColor, specularColor, shininess, refractionIndex);
    if(filename[0] !=0){
        answer->loadTexture(filename);
    }
    if (noise) {
        answer->setNoise(*noise);
        delete noise;
    }
    return answer;
}

Noise * SceneParser::parseNoise()
{
    char token[MAX_PARSER_TOKEN_LENGTH];
	Vector3f color[2];
	int colorIdx = 0;
	int octaves=0;
	float frequency  = 1;
	float amplitude = 1;
	getToken(token); assert (!strcmp(token, "{"));
	Noise *noise =0;
    while (1) {
        getToken(token); 
        if (strcmp(token, "color")==0) {
			if(colorIdx > 1){
				printf("Error parsing noise\n");
			}else{
				color[colorIdx]= readVector3f();
				colorIdx++;
			}
        }
		else if (strcmp(token, "octaves")==0) {
            octaves= readInt();
        }
		else if (strcmp(token, "frequency")==0) {
            frequency= readFloat();
        }
		else if (strcmp(token, "amplitude")==0) {
            amplitude= readFloat();
        }
		else {
            assert (!strcmp(token, "}"));
            break;
        }
    }
	return new Noise(octaves, color[0],color[1],frequency,amplitude);
}

// ====================================================================
// ====================================================================

Object3D *
SceneParser::parseObject(char token[MAX_PARSER_TOKEN_LENGTH]) 
{
    Object3D *answer = NULL;
    if (!strcmp(token, "Group")) {            
        answer = (Object3D*)parseGroup();
    } else if (!strcmp(token, "Sphere")) {            
        answer = (Object3D*)parseSphere();
    } else if (!strcmp(token, "Plane")) {            
        answer = (Object3D*)parsePlane();
    } else if (!strcmp(token, "Triangle")) {            
        answer = (Object3D*)parseTriangle();
    } else if (!strcmp(token, "TriangleMesh")) {            
        answer = (Object3D*)parseTriangleMesh();
    } else if (!strcmp(token, "Transform")) {            
        answer = (Object3D*)parseTransform();
    } else {
        printf ("Unknown token in parseObject: '%s'\n", token);
        exit(0);
    }
    return answer;
}

// ====================================================================
// ====================================================================

Group *
SceneParser::parseGroup() 
{
    //
    // each group starts with an integer that specifies
    // the number of objects in the group
    //
    // the material index sets the material of all objects which follow,
    // until the next material index (scoping for the materials is very
    // simple, and essentially ignores any tree hierarchy)
    //
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token); assert(!strcmp(token, "{"));

    // read in the number of objects
    getToken(token); assert(!strcmp(token, "numObjects"));
    int num_objects = readInt();

    Group *answer = new Group();

    // read in the objects
    int count = 0;
    while (num_objects > count) {
        getToken(token); 
        if (!strcmp(token, "MaterialIndex")) {
            // change the current material
            int index = readInt();
            assert(index >= 0 && index <= getNumMaterials());
            _current_material = getMaterial(index);
        } else {
            Object3D *object = parseObject(token);
            assert(object != NULL);
            answer->addObject(object);

            count++;
        }
    }
    getToken(token); assert(!strcmp(token, "}"));

    // return the group
    return answer;
}

// ====================================================================
// ====================================================================

Sphere *
SceneParser::parseSphere() 
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token); assert(!strcmp(token, "{"));
    getToken(token); assert(!strcmp(token, "center"));
    Vector3f center = readVector3f();
    getToken(token); assert(!strcmp(token, "radius"));
    float radius = readFloat();
    getToken(token); assert(!strcmp(token, "}"));
    assert(_current_material != NULL);
    return new Sphere(center,radius,_current_material);
}

Plane *
SceneParser::parsePlane() 
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token); assert(!strcmp(token, "{"));
    getToken(token); assert(!strcmp(token, "normal"));
    Vector3f normal = readVector3f();
    getToken(token); assert(!strcmp(token, "offset"));
    float offset = readFloat();
    getToken(token); assert(!strcmp(token, "}"));
    assert(_current_material != NULL);
    return new Plane(normal,offset,_current_material);
}

Triangle *
SceneParser::parseTriangle() 
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    getToken(token); assert(!strcmp(token, "{"));
    getToken(token); 
    assert(!strcmp(token, "vertex0"));
    Vector3f v0 = readVector3f();
    getToken(token); 
    assert(!strcmp(token, "vertex1"));
    Vector3f v1 = readVector3f();
    getToken(token); 
    assert(!strcmp(token, "vertex2"));
    Vector3f v2 = readVector3f();
    getToken(token); assert(!strcmp(token, "}"));
    assert(_current_material != NULL);
    Vector3f a = v1 - v0;
    Vector3f b = v2 - v0;
    Vector3f n = Vector3f::cross(a, b).normalized();
    return new Triangle(v0, v1, v2, n, n, n, _current_material);
}

Mesh *
SceneParser::parseTriangleMesh() 
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    char filename[MAX_PARSER_TOKEN_LENGTH];
    // get the filename
    getToken(token); assert(!strcmp(token, "{"));
    getToken(token); assert(!strcmp(token, "obj_file"));
    getToken(filename); 
    getToken(token); assert(!strcmp(token, "}"));
    const char *ext = &filename[strlen(filename)-4];
    assert(!strcmp(ext,".obj"));
    Mesh *answer = new Mesh(filename,_current_material);

    return answer;
}

Transform *
SceneParser::parseTransform() 
{
    char token[MAX_PARSER_TOKEN_LENGTH];
    Matrix4f matrix = Matrix4f::identity();
    Object3D *object = NULL;
    getToken(token); assert(!strcmp(token, "{"));
    // read in transformations: 
    // apply to the LEFT side of the current matrix (so the first
    // transform in the list is the last applied to the object)
    getToken(token);

    while (true) {
        if (!strcmp(token,"Scale")) {
            Vector3f s = readVector3f();
            matrix = matrix * Matrix4f::scaling( s[0], s[1], s[2] );
        } else if (!strcmp(token,"UniformScale")) {
            float s = readFloat();
            matrix = matrix * Matrix4f::uniformScaling( s );
        } else if (!strcmp(token,"Translate")) {
            matrix = matrix * Matrix4f::translation( readVector3f() );
        } else if (!strcmp(token,"XRotate")) {
            matrix = matrix * Matrix4f::rotateX((float) DegreesToRadians(readFloat()));
        } else if (!strcmp(token,"YRotate")) {
            matrix = matrix * Matrix4f::rotateY((float) DegreesToRadians(readFloat()));
        } else if (!strcmp(token,"ZRotate")) {
            matrix = matrix * Matrix4f::rotateZ((float) DegreesToRadians(readFloat()));
        } else if (!strcmp(token,"Rotate")) {
            getToken(token); assert(!strcmp(token, "{"));
            Vector3f axis = readVector3f();
            float degrees = readFloat();
            float radians = (float) DegreesToRadians(degrees);
            matrix = matrix * Matrix4f::rotation(axis,radians);
            getToken(token); assert(!strcmp(token, "}"));
        } else if (!strcmp(token,"Matrix4f")) {
            Matrix4f matrix2 = Matrix4f::identity();
            getToken(token); assert(!strcmp(token, "{"));
            for (int j = 0; j < 4; j++) {
                for (int i = 0; i < 4; i++) {
                    float v = readFloat();
                    matrix2( i, j ) = v; 
                } 
            }
            getToken(token); assert(!strcmp(token, "}"));
            matrix = matrix2 * matrix;
        } else {
            // otherwise this must be an object,
            // and there are no more transformations
            object = parseObject(token);
            break;
        }
        getToken(token);
    }

    assert(object != NULL);
    getToken(token); assert(!strcmp(token, "}"));
    return new Transform(matrix, object);
}

// ====================================================================
// ====================================================================

int
SceneParser::getToken(char token[MAX_PARSER_TOKEN_LENGTH]) 
{
    // for simplicity, tokens must be separated by whitespace
    assert(_file != NULL);
    int success = fscanf(_file,"%s ",token);
    if (success == EOF) {
        token[0] = '\0';
        return 0;
    }
    return 1;
}

Vector3f 
SceneParser::readVector3f() 
{
    float x,y,z;
    int count = fscanf(_file,"%f %f %f",&x,&y,&z);
    if (count != 3) {
        printf ("Error trying to read 3 floats to make a Vector3f\n");
        assert(0);
    }
    return Vector3f(x,y,z);
}


Vector2f 
SceneParser::readVec2f() 
{
    float u,v;
    int count = fscanf(_file,"%f %f",&u,&v);
    if (count != 2) {
        printf ("Error trying to read 2 floats to make a Vec2f\n");
        assert(0);
    }
    return Vector2f(u,v);
}

float 
SceneParser::readFloat() 
{
    float answer;
    int count = fscanf(_file,"%f",&answer);
    if (count != 1) {
        printf ("Error trying to read 1 float\n");
        assert(0);
    }
    return answer;
}


int 
SceneParser::readInt() 
{
    int answer;
    int count = fscanf(_file,"%d",&answer);
    if (count != 1) {
        printf ("Error trying to read 1 int\n");
        assert(0);
    }
    return answer;
}
