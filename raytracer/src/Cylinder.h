#ifndef CYLINDER_H
#define CYLINDER_H

#include "Geometry.h"

class Cylinder : public Geometry
{
public:
    Cylinder();
    virtual ~Cylinder();

    virtual void buildGeometry();

protected:
    virtual Intersect intersectImpl(const Ray &ray) const;

private:
    glm::vec3 center_;
    float radius_;
    float height_;
};

#endif