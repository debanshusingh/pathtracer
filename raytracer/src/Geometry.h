#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include "scene.h"

using namespace std;

class BBox {
public:
    BBox(){}
    BBox(vec3 b1, vec3 b2);
    BBox combine(const BBox &b);
    BBox combine(const vec3 &p);
    
    vec3 bBoxMin, bBoxMax;
    vec3 centroid(){return 0.5f*(bBoxMax + bBoxMin);}
    bool isHit(Ray r) const;
    int maximumExtent() const {
        vec3 diag = bBoxMax - bBoxMin;
        if (diag.x>diag.y && diag.x > diag.z)
            return 0;
        else if (diag.y > diag.z)
            return 1;
        else
            return 2;
    }
    
};

class Material
{
public:
    string matName;
    vec3 diffColor, specColor;
    float specExpo, ior, emittance;
    bool isMirr, isTran, isEmit;
};

class Intersect {
public:
    // The parameter `t` along the ray which was used. (A negative value indicates no intersection.)
    Intersect() : t(-1.0f), normal(glm::vec3(0,0,0)), hit(false) {}
    Intersect(float t, glm::vec3 n) : t(t), normal(n), hit(true) {}
    
    glm::vec3 normal;
    float t;
    bool hit;
    Geometry *geom;
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

// An abstract base class for geometry in the scene graph.
class Geometry
{
public:
    // Enums for the types of geometry that your scene graph is required to contain.
    // Feel free to add more.
    enum geometryType {CUBE, SPHERE, CYLINDER, MESH, TRIANGLE, BVHNODE};
    Geometry(geometryType);
    void load();
    void setColor(vec3 color);
    void setMaterial(const Material &mat);
    GLuint vao;
    GLuint vboPos;
    GLuint vboCol;
    GLuint vboNor;
    GLuint vboIdx;
    BBox bbox;
    vector<Geometry* > triangleList;

    
    // Getters
    const vector<glm::vec3>& getVertices() const
    {
        return vertices_;
    }
    const vector<glm::vec3>& getNormals() const
    {
        return normals_;
    }
    const vector<glm::vec3>& getColors() const
    {
        return colors_;
    }
    const vector<GLuint>& getIndices() const
    {
        return indices_;
    }

    GLuint getVertexCount() const
    {
        return static_cast<GLuint>(vertices_.size());
    }
    
    GLuint getIndexCount() const
    {
        return static_cast<GLuint>(indices_.size());
    }

    const geometryType getGeometryType() const
    {
        return type_;
    }

    /// Compute an intersection with a WORLD-space ray.
    Intersect intersect(const glm::mat4 &T, Ray ray_world);
    vec3 hit(Intersect isx) const;
    
    geometryType type_;

    vector<glm::vec3> vertices_;        // vertex buffer
    vector<glm::vec3> normals_;         // normal buffer
    vector<glm::vec3> colors_;          // color buffer
    vector<GLuint> indices_;      // index buffer

    virtual ~Geometry();
    // Function for building vertex data, i.e. vertices, colors, normals, indices.
    // Implemented in Sphere and Cylinder.
    virtual void buildGeometry(){};
    /// Compute an intersection with an OBJECT-LOCAL-space ray.
    virtual Intersect intersectImpl(const Ray &ray) const {return Intersect();};
    Material material;
};

#endif
