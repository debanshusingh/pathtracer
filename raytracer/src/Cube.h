#ifndef CUBE_H
#define CUBE_H

#include "Geometry.h"

class Cube : public Geometry
{
public:
    Cube();
    virtual ~Cube();
    
    virtual void buildGeometry();
    
protected:

    virtual Intersect intersectImpl(const Ray &ray) const;

};

#endif