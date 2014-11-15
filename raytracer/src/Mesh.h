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

class Mesh : public Geometry
{
public:
    Mesh();
    virtual ~Mesh();
    
    virtual void buildGeometry();
    
    virtual Intersect intersectImpl(const Ray &ray) const;
    void intersectPoly(const Ray &ray, const vec3& v0, const vec3& v1, const vec3& v2, Intersect &isx) const;
};

#endif /* defined(__raytracer__Mesh__) */
