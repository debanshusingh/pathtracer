#ifndef INTERSECTION_H
#define INTERSECTION_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

class Intersection {
    // The parameter `t` along the ray which was used. (A negative value indicates no intersection.)
    Intersection() : pos(vec3(0,0,0)), normal(vec3(0,0,0)), shape(NULL), hit(false) { }

    float t;
    // The surface normal at the point of intersection. (Ignored if t < 0.)
    glm::vec3 normal;
};


    // scratch-a-pixel
template<typename T>
bool solveQuadratic(const T &a, const T &b, const T &c, T &x0, T &x1)
{
    T discr = b * b - 4 * a * c;
    if (discr < 0) return false;
    else if (discr == 0) x0 = x1 = - 0.5 * b / a;
    else {
        T q = (b > 0) ?
        -0.5 * (b + sqrt(discr)) :
        -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1) std::swap(x0, x1);
    return true;
}

#endif
