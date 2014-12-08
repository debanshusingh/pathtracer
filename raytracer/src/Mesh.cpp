//
//  Mesh.cpp
//  raytracer
//
//  Created by Debanshu on 10/26/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#include "Mesh.h"
//#include "bvh.h"

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
    return tree->intersectImpl(ray);
}

Intersect Triangle::intersectImpl(const Ray &ray) const
{
    Intersect isx;

    // MT ray-triangle intersection algorithm
    vec3 e1 = v1-v0;
    vec3 e2 = v2-v0;
    vec3 p = glm::cross(ray.dir,e2);
    float det = glm::dot(e1,p);
    
    if (det > -EPSILON && det < EPSILON) return isx;
    
    float invDet = 1 / det;
    vec3 d = ray.pos - v0;
    
    float u = glm::dot(d,p)*invDet;
    if (u < 0 || u > 1) return isx;
    
    vec3 q = glm::cross(d, e1);
    float v = glm::dot(ray.dir,q)*invDet;
    if (v < 0 || u + v > 1) return isx;
    
    float t = glm::dot(e2, q) * invDet;
    if (t < 0) return isx;
    
    isx.t = t;
    isx.hit = true;
    isx.normal = glm::normalize(glm::cross(e1,e2));
    
    return isx;

//    think how to increment metrics
//    numRayTrianglesTests++;
}