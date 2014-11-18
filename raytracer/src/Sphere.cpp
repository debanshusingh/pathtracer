#include "Sphere.h"

//static const float PI = 3.141592653589f;

// Creates a unit sphere.
Sphere::Sphere() :
    Geometry(SPHERE),
    center_(glm::vec3(0.0f, 0.0f, 0.0f)),
    radius_(1.0f)
{
    buildGeometry();
}

Sphere::~Sphere() {}

void Sphere::buildGeometry()
{
    vertices_.clear();
    colors_.clear();
    normals_.clear();
    indices_.clear();

    // Find vertex positions for the sphere.
    GLuint subdiv_axis = 16;      // vertical slices
    GLuint subdiv_height = 16;        // horizontal slices
    float dphi = PI / subdiv_height;
    float dtheta = 2.0f * PI / subdiv_axis;
    float epsilon = 0.0001f;
    glm::vec3 color (0.6f, 0.6f, 0.6f);

    // North pole
    glm::vec3 point (0.0f, 1.0f, 0.0f);
    normals_.push_back(point);
    // scale by radius_ and translate by center_
    vertices_.push_back(center_ + radius_ * point);

    for (float phi = dphi; phi < PI; phi += dphi) {
        for (float theta = dtheta; theta <= 2.0f * PI + epsilon; theta += dtheta) {
            float sin_phi = sin(phi);

            point[0] = sin_phi * sin(theta);
            point[1] = cos(phi);
            point[2] = sin_phi * cos(theta);

            normals_.push_back(point);
            vertices_.push_back(center_ + radius_ * point);
        }
    }
    // South pole
    point = glm::vec3(0.0f, -1.0f, 0.0f);
    normals_.push_back(point);
    vertices_.push_back(center_ + radius_ * point);

    // fill in index array.
    // top cap
    for (GLuint i = 0; i < subdiv_axis - 1; ++i) {
        indices_.push_back(0);
        indices_.push_back(i + 1);
        indices_.push_back(i + 2);
    }
    indices_.push_back(0);
    indices_.push_back(subdiv_axis);
    indices_.push_back(1);

    // middle subdivs
    GLuint index = 1;
    for (GLuint i = 0; i < subdiv_height - 2; i++) {
        for (GLuint j = 0; j < subdiv_axis - 1; j++) {
            // first triangle
            indices_.push_back(index);
            indices_.push_back(index + subdiv_axis);
            indices_.push_back(index + subdiv_axis + 1);

            // second triangle
            indices_.push_back(index);
            indices_.push_back(index + subdiv_axis + 1);
            indices_.push_back(index + 1);

            index++;
        }
        // reuse vertices from start and end point of subdiv_axis slice
        indices_.push_back(index);
        indices_.push_back(index + subdiv_axis);
        indices_.push_back(index + 1);

        indices_.push_back(index);
        indices_.push_back(index + 1);
        indices_.push_back(index + 1 - subdiv_axis);

        index++;
    }

    // end cap
    GLuint bottom = (subdiv_height - 1) * subdiv_axis + 1;
    GLuint offset = bottom - subdiv_axis;
    for (GLuint i = 0; i < subdiv_axis - 1 ; ++i) {
        indices_.push_back(bottom);
        indices_.push_back(i + offset);
        indices_.push_back(i + offset + 1);
    }
    indices_.push_back(bottom);
    indices_.push_back(bottom - 1);
    indices_.push_back(offset);

}

Intersect Sphere::intersectImpl(const Ray &ray) const {
    
    // break the ray into position and direction
    // ray p = p_o + p_d (t)
    
    // a sphere is parametrically defined by radius and position
    // (point - Center) * (point - center ) - radius ^2 = 0

    Intersect isx;

    float a = glm::dot(ray.dir, ray.dir);
    float b = 2 * glm::dot(ray.dir, (ray.pos - center_));
    float c = glm::dot((ray.pos - center_),(ray.pos - center_)) - pow(radius_,2.0f);
    
    float t0, t1;
    
    if (!solveQuadratic(a, b, c, t0, t1)) isx.t = -1;
    else {
        if (t0 < 0) {
            if (t1 > 0) {
                isx.t = t1;
            }
            else isx.t = -1;
        }
        else isx.t = t0;

        vec3 p = ray.pos + isx.t * ray.dir;
        vec3 normal_local = p - center_;
        isx.normal = normal_local;
    }
    return isx;
}