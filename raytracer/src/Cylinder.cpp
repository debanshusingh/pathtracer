#include "Cylinder.h"

// Creates a unit cylinder centered at (0, 0, 0)
Cylinder::Cylinder() :
Geometry(CYLINDER),
center_(glm::vec3(0.f, 0.f, 0.f)),
radius_(0.5f),
height_(1.0f)
{
    buildGeometry();
}

Cylinder::~Cylinder() {}

void Cylinder::buildGeometry()
{
    vertices_.clear();
    colors_.clear();
    normals_.clear();
    indices_.clear();
    
    unsigned short subdiv = 20;
    float dtheta = 2 * PI / subdiv;
    
    glm::vec4 point_top(radius_, 0.5f * height_, radius_, 1.0f),
    point_bottom (radius_, -0.5f * height_, radius_, 1.0f);
    vector<glm::vec3> cap_top, cap_bottom;
    
    // top and bottom cap vertices
    for (int i = 0; i < subdiv + 1; ++i) {
        glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::degrees(i * dtheta), glm::vec3(0, 1, 0));
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), center_);

        cap_top.push_back(glm::vec3(translate * rotate * point_top));
        cap_bottom.push_back(glm::vec3(translate * rotate * point_bottom));
    }

    //Create top cap.
    for ( int i = 0; i < subdiv - 2; i++) {
        vertices_.push_back(cap_top[0]);
        vertices_.push_back(cap_top[i + 1]);
        vertices_.push_back(cap_top[i + 2]);
    }
    //Create bottom cap.
    for (int i = 0; i < subdiv - 2; i++) {
        vertices_.push_back(cap_bottom[0]);
        vertices_.push_back(cap_bottom[i + 1]);
        vertices_.push_back(cap_bottom[i + 2]);
    }
    //Create barrel
    for (int i = 0; i < subdiv; i++) {
        //Right-side up triangle
        vertices_.push_back(cap_top[i]);
        vertices_.push_back(cap_bottom[i + 1]);
        vertices_.push_back(cap_bottom[i]);

        //Upside-down triangle
        vertices_.push_back(cap_top[i]);
        vertices_.push_back(cap_top[i + 1]);
        vertices_.push_back(cap_bottom[i + 1]);
    }
    
    // create normals
    glm::vec3 top_centerpoint(0.0f , 0.5f * height_ , 0.0f), bottom_centerpoint(0.0f, -0.5f * height_, 0.0f);
    glm::vec3 normal(0, 1, 0);
    
    // Create top cap.
    for (int i = 0; i < subdiv - 2; i++) {
        normals_.push_back(normal);
        normals_.push_back(normal);
        normals_.push_back(normal);
    }
    // Create bottom cap.
    for (int i = 0; i < subdiv - 2; i++) {
        normals_.push_back(-normal);
        normals_.push_back(-normal);
        normals_.push_back(-normal);
    }
    
    // Create barrel
    for (int i = 0; i < subdiv; i++) {
        //Right-side up triangle
        normals_.push_back(glm::normalize(cap_top[i] - top_centerpoint));
        normals_.push_back(glm::normalize(cap_bottom[i + 1] - bottom_centerpoint));
        normals_.push_back(glm::normalize(cap_bottom[i] - bottom_centerpoint));
        //Upside-down triangle
        normals_.push_back(glm::normalize(cap_top[i] - top_centerpoint));
        normals_.push_back(glm::normalize(cap_top[i + 1] - top_centerpoint));
        normals_.push_back(glm::normalize(cap_bottom[i + 1] - bottom_centerpoint));
    }
    
    // indices and colors
    
    for (unsigned int i = 0; i < vertices_.size(); ++i) {
        indices_.push_back(i);
    }
}

Intersect Cylinder::intersectImpl(const Ray &ray) const {
    Intersect isx;
    vec3 axis(0, 1, 0);
    vec3 deltaP = ray.pos - center_;
    glm::vec3 point_top(radius_, 0.5f * height_, radius_), point_bottom(radius_, -0.5f * height_, radius_);

    float a = glm::dot((ray.dir - glm::dot(ray.dir, axis)*axis),(ray.dir - glm::dot(ray.dir, axis)*axis));
    float b = 2 * glm::dot((ray.dir - glm::dot(ray.dir, axis)*axis), (deltaP - glm::dot(deltaP,axis)*axis));
    float c = glm::dot((deltaP - glm::dot(deltaP,axis)*axis),(deltaP - glm::dot(deltaP,axis)*axis)) - pow(radius_,2.0f);

    float t0, t1;
    if (glm::length(glm::cross(ray.dir, axis)) == 0) {
        
        if (glm::sign(glm::dot(ray.dir, axis)) < 0) {
            // top cap
            vec3 N = vec3(0,1,0);
            float nDotR = glm::dot(ray.dir, N);
            
            // check if ray and plane are parallel
            if (nDotR == 0) return isx;
            
            float D = glm::dot(N,point_top);
            float t = (D-(glm::dot(N, ray.pos)))/nDotR;
            
            // check if triangle is behind the ray
            if (t < 0) return isx;
            
            isx.t = t;
            isx.normal = -N;

        }
        else {
            // bottom cap
            
            vec3 N = vec3(0,-1,0);
            float nDotR = glm::dot(ray.dir, N);
            
            // check if ray and plane are parallel
            if (nDotR == 0) return isx;
            
            float D = glm::dot(N,point_bottom);
            float t = (D-(glm::dot(N, ray.pos)))/nDotR;
            
            // check if triangle is behind the ray
            if (t < 0) return isx;
            
            isx.t = t;
            isx.normal = -N;            
        }

    }
    else if (!solveQuadratic(a, b, c, t0, t1)) isx.t = -1;
    else {
        vec3 p0 = ray.pos + t0 * ray.dir;
        vec3 p1 = ray.pos + t1 * ray.dir;
        if ((p0.y<point_top.y) && (p0.y>point_bottom.y)){
            // cylinder wall

            // check if triangle is behind the ray
            if (t0 < 0) {
                if (t0 + ZERO_ABSORPTION_EPSILON >= 0) {
                    t0 = 0;
                    isx.t = t0;
                    return isx;
                }
                else if (t1<0) {
                    if (t1 + ZERO_ABSORPTION_EPSILON >= 0) {
                        t1 = 0;
                        isx.t = t1;
                        return isx;
                    }
                }
                else isx.t = t1;
            }
            else isx.t = t0;

            vec3 v0 = p0-center_; vec3 v1 = glm::dot(p1,axis)*axis;
            isx.normal = v0-v1;
        }
        if ((p0.y>point_top.y) && (p1.y<point_top.y)){
            // top cap
            
            vec3 N = vec3(0,1,0);
            float nDotR = glm::dot(ray.dir, N);
            
            // check if ray and plane are parallel
            if (nDotR == 0) return isx;
            
            float D = glm::dot(N,point_top);
            float t = (D-(glm::dot(N, ray.pos)))/nDotR;
            
            // check if triangle is behind the ray
            if (t < 0) return isx;

            isx.t = t;
            isx.normal = N;
        }
        if ((p0.y<point_bottom.y) && (p1.y>point_bottom.y)){
            // bottom cap
            
            vec3 N = vec3(0,-1,0);
            float nDotR = glm::dot(ray.dir, N);
            
            // check if ray and plane are parallel
            if (nDotR == 0) return isx;
            
            float D = glm::dot(N,point_bottom);
            float t = (D-(glm::dot(N, ray.pos)))/nDotR;
            
            // check if triangle is behind the ray
            if (t < 0) return isx;
            
            isx.t = t;
            isx.normal = -N;
        }
    }
    return isx;
}