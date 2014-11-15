//
//  Mesh.cpp
//  raytracer
//
//  Created by Debanshu on 10/26/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#include "Mesh.h"

Mesh::Mesh() : Geometry(MESH)
{
    buildGeometry();
}

Mesh::~Mesh() {}

void Mesh::buildGeometry()
{
    vertices_.clear();
    colors_.clear();
    normals_.clear();
    indices_.clear();
}

Intersect Mesh::intersectImpl(const Ray &ray) const {
    Intersect finalIsx;
    if (getIndexCount()%3 !=0) cerr<<"Mesh vertexcount should be 3x"<<endl;
    unsigned int trianglesInMesh = getIndexCount()/3;
    for (int i=0; i<trianglesInMesh; i++) {
        Intersect isx;
        intersectPoly(ray, vertices_.at(indices_.at(3*i)), vertices_.at(indices_.at(3*i+1)), vertices_.at(indices_.at(3*i+2)), isx);
        if (isx.t >0){
            if ((finalIsx.t == -1) || (isx.t < finalIsx.t)) finalIsx = isx;
        }
    }
    return finalIsx;
}

void Mesh::intersectPoly(const Ray &ray, const vec3 &v0, const vec3 &v1, const vec3 &v2, Intersect &isx) const
{
    vec3 N = glm::cross((v1 - v0),(v2 - v0));
    float nDotR = glm::dot(ray.dir, N);

    // check if ray and plane are parallel
    if (nDotR == 0) return;
    
    float D = glm::dot(N,v0);
    float t = (D-(glm::dot(N, ray.pos)))/nDotR;
    
    // check if triangle is behind the ray
    if (t < 0) return;
    // check if point is inside triangle
    vec3 P = ray.pos + t * ray.dir;
    vec3 edge0 = v1-v0;
    vec3 edge1 = v2-v1;
    vec3 edge2 = v0-v2;
    
    if(glm::dot(N, glm::cross(edge0, P-v0)) < 0) return;
    if(glm::dot(N, glm::cross(edge1, P-v1)) < 0) return;
    if(glm::dot(N, glm::cross(edge2, P-v2)) < 0) return;
    
    // point is inside triangle
    isx.t = t;
    isx.normal = glm::normalize(N);
    isx.hit = true;
}