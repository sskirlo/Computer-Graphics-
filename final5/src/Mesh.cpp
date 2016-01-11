#include "Mesh.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>

Mesh::Mesh(const std::string &filename, Material *material) :
    Object3D(material)
{
    std::ifstream f;
    f.open(filename.c_str());
    if (!f.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }

    std::vector<Vector3f> v;
    std::vector<ObjTriangle> t;
    std::vector<Vector3f> n;
    std::vector<Vector2f> texCoord; 

    const std::string vTok("v");
    const std::string fTok("f");
    const std::string texTok("vt");
    const char bslash = '/', space=' ';
    std::string tok;
    std::string line;
    while (true) {
        std::getline(f, line);
        if (f.eof()) {
            break;
        }
        if (line.size() < 3) {
            continue;
        }
        if (line.at(0) == '#') {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok) {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            v.push_back(vec);
        } else if (tok == fTok) {
            if (line.find(bslash) != std::string::npos) {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                ObjTriangle trig;
                facess >> tok;
                for(int ii=0; ii<3; ii++) {
                    facess >> trig[ii] >> trig.texID[ii];
                    trig[ii]--;
                    trig.texID[ii]--;
                }
                t.push_back(trig);
            } else {
                ObjTriangle trig;
                for (int ii = 0; ii < 3; ii++) {
                    ss >> trig[ii];
                    trig[ii]--;
                    trig.texID[ii] = 0;
                }
                t.push_back(trig);
            }
        } else if(tok == texTok) {
            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
            texCoord.push_back(texcoord);
        }
    }
    f.close();

    // Compute normals
    //
    // XXX-HACK: Assumes mesh is smooth and thus needs normal averaging if
    // there are more than 500 vertices. This should likely be a parameter
    // to the mesh in the scene file or a global parameter specified on the
    // command line.
    bool isSmooth = v.size() > 500;
    if (isSmooth) {
        n.resize(v.size());
        for (size_t ii = 0; ii < t.size(); ii++) {
            Vector3f a = v[t[ii][1]] - v[t[ii][0]];
            Vector3f b = v[t[ii][2]] - v[t[ii][0]];
            Vector3f normal = Vector3f::cross(a,b);
            for (size_t jj = 0; jj < 3; jj++) {
                n[t[ii][jj]] += normal;
            }
        }
        for (size_t ii = 0; ii < v.size(); ii++) {
            n[ii] = n[ii] / n[ii].abs();
        }
    } else {
        n.resize(t.size());
        for (size_t ii = 0; ii < t.size(); ii++) {
            Vector3f a = v[t[ii][1]] - v[t[ii][0]];
            Vector3f b = v[t[ii][2]] - v[t[ii][0]];
            Vector3f normal = Vector3f::cross(a,b).normalized();
            n[ii] = normal;
        }
    }

    // Set up triangles
    for (size_t i = 0; i < t.size(); i++) {
        Triangle triangle(v[t[i][0]],
                          v[t[i][1]],
                          v[t[i][2]],
                          isSmooth ? n[t[i][0]] : n[i],
                          isSmooth ? n[t[i][1]] : n[i],
                          isSmooth ? n[t[i][2]] : n[i],
                          getMaterial());
        if (texCoord.size() > 0) {
            triangle.setTex(texCoord[t[i].texID[0]],
                            texCoord[t[i].texID[1]],
                            texCoord[t[i].texID[2]]);
        }
        _triangles.push_back(triangle);
    }

    octree.build(this);
}

bool
Mesh::intersect(const Ray &r, float tmin, Hit &h) const
{
    ray = &r;
    hit = &h;
    tm = tmin;
    bool result = octree.intersect(r);
    return result;
}

bool
Mesh::intersectTrig(int idx) const
{
    const Triangle &triangle = _triangles[idx];
    bool result = triangle.intersect(*ray, tm, *hit);
    return result;
}
