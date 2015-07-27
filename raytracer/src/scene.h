//
//  scene.h
//  raytracer
//
//  Created by Debanshu on 10/17/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#ifndef __raytracer__scene__
#define __raytracer__scene__


#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <stack>
#include <map>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utilities.h"
#include "EasyBMP/EasyBMP.h"

#ifdef _OPENMP
	#include <omp.h>
#else
	#define omp_get_num_threads() 0
#endif

class Scene;
class Node;
class Geometry;
class Camera;
class Ray;
class Film;
class Raytracer;
class Intersect;
class Material;
class BVH;
class BBox;
class Mesh;

// Global scene
extern Scene* scene;


static const float PI = 3.141592653589f;

using namespace glm;
using namespace std;

inline bool fileExists (const string& name) {
    ifstream f(name.c_str());
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }
}

class Camera
{
public:
    Camera(){};
    void setFrame();
    Ray generateRay(vec2 pixel);
    vec3 eye,center,up;
    vec3 u,v,w;
    float fovy,aspectRatio;
};

class Film
{
public:
    Film();
    void put(uvec2 pixel, vec3 color);
    vec3 get(uvec2 pixel);
    void writeImage(string path);
    
private:
    vector<vector<vec3>> pixels;
};

class Ray
{
public:
    Ray(){transmittance = vec3(1,1,1);}
    Ray(const glm::vec3 p, const glm::vec3 d) : pos(p), dir(d), inside(false){transmittance = vec3(1,1,1);}
    Ray(const glm::vec3 p, const glm::vec3 d, const glm::vec3 t) : pos(p), dir(d), transmittance(t){}
    Ray(const glm::vec3 p, const glm::vec3 d, bool inside) : pos(p), dir(d), inside(inside){transmittance = vec3(1,1,1);}
    Ray(const glm::vec3 p, const glm::vec3 d, bool inside, const glm::vec3 t) : pos(p), dir(d), inside(inside), transmittance(t){}
    bool inside;
    vec3 pos;
    vec3 dir;
    vec3 transmittance;
};

class Raytracer
{
public:
    Raytracer(){}
    vec3 trace(Ray &r, int depth);
    bool inShadow(vec3 isxPos, vec3 lightPos, Geometry* geom);
    
};

class Scene
{
public:
    Scene();
    void draw();
    void load();
    void render();
    
    void addNode(Node* node);
    int getTotalPrims(){return totalPrims;}
    int getHeight(){return HEIGHT;}
    int getWidth(){return WIDTH;}
    float getFov(){return fov;}
    vec3 getLightPos();
    vector<Node *> nodes;
    void parseScene(string inFilePath);
    void printLinkInfoLog(int);
    void printShaderInfoLog(int);
    void printGLErrorLog();
    string textFileRead(const char*);
    void initShader();
    void cleanup();
    void parseObj(Mesh* mesh, const string &inFilePath, vector<Geometry*> &triangleList);
    
    GLuint shaderProgram;
    GLuint locationPos;
    GLuint locationCol;
    GLuint locationNor;
    GLuint unifViewProj;
    GLuint unifModel;
    GLuint unifModelInvTr;
    GLuint unifLightPos;
    GLuint unifLightColor;
    GLuint unifCamPos;
    
    vec3 eye, center, up;
    vec3 lightPos;
    vec3 lightColor;

    float fov;
    Camera* camera;
    Film* film;
//    BVH* tree;
    
    Raytracer* raytracer;
    stack<mat4> transformations;
    bool isMonteCarlo;
    int maxDepth;
	void updateGlobalTransform(vector<Node*> nodes);

private:
    ifstream inFilePointer;
    int WIDTH, HEIGHT;
    int totalPrims;
    string inFilePath;
    Geometry* geometry;
    std::map<string,bool> geomTypes;
    std::map<string, Material> matDict;
    string outFilePath;
};

class Node
{
public:
    Node(){}
    Node(string nodeName);
    Node(Geometry* geometry, string nodeName);
    Node(Node* parent, Geometry* geometry, string nodeName);
    friend bool operator== ( Node& n1, Node& n2) {return n1.nodeName == n2.nodeName;}

    virtual ~Node(void);
    
    Intersect intersect(const Ray &r);
    void draw(stack<mat4> transformations);
    void load();
    void initNode(Geometry* Geometry);
    void addChildNode(Node* ChildNode);
    void removeChildNode(Node* ChildNode);
    Node* getParentNode(){return parent;}
    void setParentNode(Node* NewParent);
    vector<Node*> getChildren(){return children;}
    void setTransformation(mat4 transf){localTransformation = transf;}
    mat4 getTransformation(){return localTransformation;}
    mat4 getGlobalTransformation();
    Geometry* getGeometry(){return geometry;}
    string getNodeName(){return nodeName;}
    void setColor(vec3 nodeColor){tcolor = color = nodeColor;}
    vec3 getColor(){return color;}
    const bool isRootNode(void) const;
    const bool isLeafNode(void) const;
    vec3 color;
    vec3 tcolor;
    mat4 globalTransformation;
    
private:
    string nodeName;
    Node* parent;
    vector<Node*> children;
    mat4 localTransformation;
    Geometry* geometry;

};

#endif /* defined(__raytracer__scene__) */
