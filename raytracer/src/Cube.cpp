//
//  Cube.cpp
//  raytracer
//
//  Created by Debanshu on 10/15/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#include "Cube.h"

// Creates a unit cube.
Cube::Cube() : Geometry(CUBE)
{
    buildGeometry();
}

Cube::~Cube() {}

void Cube::buildGeometry()
{
    vertices_.clear();
    colors_.clear();
    normals_.clear();
    indices_.clear();
    
    // Find vertex positions for the cube.
    
    GLfloat vertices[] = {
        1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,   // v0,v1,v2,v3 (front)
        1, 1, 1,   1,-1, 1,   1,-1,-1,   1, 1,-1,   // v0,v3,v4,v5 (right)
        1, 1, 1,   1, 1,-1,  -1, 1,-1,  -1, 1, 1,   // v0,v5,v6,v1 (top)
        -1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,   // v1,v6,v7,v2 (left)
        -1,-1,-1,   1,-1,-1,   1,-1, 1,  -1,-1, 1,   // v7,v4,v3,v2 (bottom)
        1,-1,-1,  -1,-1,-1,  -1, 1,-1,   1, 1,-1 }; // v4,v7,v6,v5 (back)
    
    for (int i=0; i<24*3; i+=3) {
        vertices_.push_back(glm::vec3(vertices[i]/2,vertices[i+1]/2,vertices[i+2]/2));
    }
    
    // normal array
    GLfloat normals[]  = {
        0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   // v0,v1,v2,v3 (front)
        1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,   // v0,v3,v4,v5 (right)
        0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,   // v0,v5,v6,v1 (top)
        -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,   // v1,v6,v7,v2 (left)
        0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,   // v7,v4,v3,v2 (bottom)
        0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1 }; // v4,v7,v6,v5 (back)
    
    for (int i=0; i<24*3; i+=3) {
        normals_.push_back(glm::vec3(normals[i],normals[i+1],normals[i+2]));
    }
        
    GLuint indices[]  = {
        0, 1, 2,   2, 3, 0,      // front
        4, 5, 6,   6, 7, 4,      // right
        8, 9,10,  10,11, 8,      // top
        12,13,14,  14,15,12,      // left
        16,17,18,  18,19,16,      // bottom
        20,21,22,  22,23,20 };    // back
    
    for (int i=0; i<36; i++) {
        indices_.push_back(indices[i]);
    }

}

Intersect Cube::intersectImpl(const Ray &ray) const {
    Intersect isx;
    float tnear = - numeric_limits<float>::max();
    float tfar = numeric_limits<float>::max();
    float t1,t2,temp;
    
    vec3 boxMin = vec3(-0.5f,-0.5f,-0.5f);
    vec3 boxMax = vec3(0.5f,0.5f,0.5f);
    
    isx.hit = true;
    for(int i =0 ;i < 3; i++){
        if(ray.dir[i] == 0){
            if(ray.pos[i] < boxMin[i] || ray.pos[i] > boxMax[i])
                isx.hit = false;
        }
        else{
            t1 = (boxMin[i] - ray.pos[i])/ray.dir[i];
            t2 = (boxMax[i] - ray.pos[i])/ray.dir[i];
            if(t1 > t2){
                temp = t1;
                t1 = t2;
                t2 = temp;
            }
            if(t1 > tnear)
                tnear = t1;
            if(t2 < tfar)
                tfar = t2;
            if(tnear > tfar)
                isx.hit = false;
            if(tfar < 0)
                isx.hit = false;
        }
    }
    if(isx.hit == false)
        return isx;
    else
        isx.t = tnear;
    
    vec3 isxPos = ray.pos + tnear*ray.dir;
    
    float EPS = 0.001f;
    
    if(abs(isxPos[0] - boxMin[0]) < EPS)
        isx.normal = vec3(-1,0,0);
    else if(abs(isxPos[0] - boxMax[0]) < EPS)
        isx.normal = vec3(1,0,0);
    else if(abs(isxPos[1] - boxMin[1]) < EPS)
        isx.normal = vec3(0,-1,0);
    else if(abs(isxPos[1] - boxMax[1]) < EPS)
        isx.normal = vec3(0,1,0);
    else if(abs(isxPos[2] - boxMin[2]) < EPS)
        isx.normal = vec3(0,0,-1);
    else if(abs(isxPos[2] - boxMax[2]) < EPS)
        isx.normal = vec3(0,0,1);

    return isx;
}