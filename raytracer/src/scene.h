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


class Scene;
class Node;
class Geometry;
class Camera;
class Ray;
class Film;
class Raytracer;
class Intersect;
class Material;

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
    Camera(vec3 eye, vec3 center, vec3 up, float fovy): eye(eye), center(center), up(up), fovy(fovy){};
    Ray generateRay(uvec2 pixel);
    vec3 eye,center,up;
    vec3 A,B,C;
    float fovy,fovx;
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
    Ray(){}
    Ray(const glm::vec3 p, const glm::vec3 d) : pos(p), dir(d){}
    vec3 pos;
    vec3 dir;
};

class Raytracer
{
public:
    Raytracer(){}
    vec3 trace(const Ray &r);
};

class Scene
{
public:
    Scene();
    void init();
    void draw();
    void load();
    void render();
    
    void addNode(Node* node);
    void updateNode(Node* node);
    void deleteNode(Node* node);
    int getTotalPrims(){return totalPrims;}
    int getHeight(){return HEIGHT;}
    int getWidth(){return WIDTH;}
    float getFov(){return fov;}
    vec3 getLightPos(){return lightPos;}
    vector<Node *> nodes;
    void parseScene(string inFilePath);
    void printLinkInfoLog(int);
    void printShaderInfoLog(int);
    void printGLErrorLog();
    string textFileRead(const char*);
    void initShader();
    void cleanup();
    
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
    Node* currentNode;
    Node* previousNode;
    Camera* camera;
    Film* film;
    Raytracer* raytracer;
    stack<mat4> transformations;

private:
    ifstream inFilePointer;
    int WIDTH, HEIGHT;
    int totalPrims;
    string inFilePath;
    Geometry* geometry;
    std::map<string,bool> geomTypes;
    std::map<string,Material> matDict;
    string outFilePath;
};

class Node
{
public:
    Node(string nodeName);
    Node(Geometry* geometry, string nodeName);
    Node(Node* parent, Geometry* geometry, string nodeName);
    friend bool operator== ( Node& n1, Node& n2) {return n1.nodeName == n2.nodeName;}

    virtual ~Node(void);
    
    Intersect intersect(stack<mat4> &transformations, const Ray &r);
    void draw(stack<mat4> transformations);
    void load();
    void initNode(Geometry* Geometry);
    virtual void Update(int type, float n);
    void addChildNode(Node* ChildNode);
    void removeChildNode(Node* ChildNode);
    Node* getParentNode(){return parent;}
    void setParentNode(Node* NewParent);
    vector<Node*> getChildren(){return children;}
    void setTransformation(mat4 transf){transformation = transf;}
    mat4 getTransformation(){return transformation;}
    Geometry* getGeometry(){return geometry;}
    string getNodeName(){return nodeName;}
    void setColor(vec3 nodeColor){tcolor = color = nodeColor;}
    vec3 getColor(){return color;}
    const bool isRootNode(void) const;
    const bool isLeafNode(void) const;
    vec3 color;
    vec3 tcolor;

private:
    string nodeName;
    Node* parent;
    vector<Node*> children;
    mat4 transformation;
    Geometry* geometry;
};

#endif /* defined(__raytracer__scene__) */
