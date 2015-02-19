#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include "scene.h"
#include "Intersect.h"


using namespace std;

class Material
{
public:
    string matName;
    vec3 diffColor, specColor;
    float specExpo, ior, emittance;
    bool isMirr, isTran, isEmit;
};

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
    BVHNode* tree;

    
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
    virtual void buildGeometry() = 0;
    /// Compute an intersection with an OBJECT-LOCAL-space ray.
    virtual Intersect intersectImpl(const Ray &ray) const = 0;
    Material material;
};

#endif
