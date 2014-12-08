//
//  Mesh.h
//  raytracer
//
//  Created by Debanshu on 10/26/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#ifndef __raytracer__Mesh__
#define __raytracer__Mesh__

#include "Geometry.h"

class Triangle : public Geometry
{
public:
    Triangle(): Geometry(TRIANGLE){};
    ~Triangle(){};
    Triangle(vec3 v0, vec3 v1, vec3 v2): v0(v0), v1(v1), v2(v2), Geometry(TRIANGLE) {};
    virtual Intersect intersectImpl(const Ray &ray) const;
    virtual void buildGeometry(){};

    vec3 v0,v1,v2;
};

class BVHNode : public Geometry
{
public:
    BVHNode(BBox b, Geometry* left, Geometry* right): left(left), right(right), Geometry(BVHNODE) {bbox = b;}
    
    virtual ~BVHNode(){};
    virtual void buildGeometry(){};
    virtual Intersect intersectImpl(const Ray &ray) const;
    Geometry* left;
    Geometry* right;
    BBox bbox;
    
};

class Mesh : public Geometry
{
public:
    Mesh();
    virtual ~Mesh();
    virtual void buildGeometry();
    virtual Intersect intersectImpl(const Ray &ray) const;
        
};

#endif /* defined(__raytracer__Mesh__) */
