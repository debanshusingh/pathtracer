#ifndef SPHERE_H
#define SPHERE_H

#include "Geometry.h"

class Sphere : public Geometry
{
public:
    Sphere();
    ~Sphere();

    virtual void buildGeometry();

protected:
    virtual Intersect intersectImpl(const Ray &ray) const;

private:
    glm::vec3 center_;
    float radius_;
};

#endif