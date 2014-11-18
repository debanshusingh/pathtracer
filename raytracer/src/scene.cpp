//
//  scene.cpp
//  raytracer
//
//  Created by Debanshu on 10/17/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#include "scene.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Cube.h"
#include "Mesh.h"
#include "Intersect.h"

Scene::Scene(){
    geomTypes["CUBE"] = false;
	geomTypes["SPHERE"] = false;
	geomTypes["CYLINDER"] = false;
	geomTypes["MESH"] = false;
    maxDepth = 5;
}

void Scene::parseScene(string inFilePath){
    /****************************************/
    
    /***********  PARSE SCENE ***************/

    inFilePointer.open((char*)inFilePath.c_str());
    if (inFilePointer.is_open()){
        cout<<"[raytracer] Reading scene from "<< inFilePath <<endl;
        while (inFilePointer.good()) {
            string line;
            utilityCore::safeGetline(inFilePointer, line);
            cout<<line<<endl;
            if ((line.find_first_not_of(" \t\r\n") != string::npos) && (line[0] != '#')) {

                vector<string> tokens = utilityCore::tokenizeString(line);
                if(strcmp(tokens[0].c_str(), "CAMERA")==0){
                    camera = new Camera(); // initiate arbitrary camera
                    
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if(strcmp(tokens[0].c_str(), "RESO")==0){
                        WIDTH = atoi(tokens[1].c_str());
                        HEIGHT = atoi(tokens[2].c_str());
                    }
                    camera->aspectRatio = (float)WIDTH/HEIGHT;
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "EYEP")==0){
                        camera->eye = vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                    }
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "VDIR")==0){
                        camera->center = camera->eye + vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                    }
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "UVEC")==0){
                        camera->up = vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                    }
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "FOVY")==0){
                        camera->fovy = stof(tokens[1].c_str());
                    }
                    camera->setFrame();
                }
                
                else if(strcmp(tokens[0].c_str(), "LIGHT")==0){

                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "LPOS")==0){
                        lightPos = vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                    }
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "LCOL")==0){
                        lightColor = vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                    }
                }
                
                else if(strcmp(tokens[0].c_str(), "MAT")==0){
                    
                    tokens = utilityCore::tokenizeString(line);
                    string matName = tokens[1];
                    Material material;
                    material.matName = matName;
                    
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "DIFF")==0){
                        material.diffColor = vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                    }
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "REFL")==0){
                        material.specColor = vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                    }
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "EXPO")==0){
                        material.specExpo = stof(tokens[1]);
                    }
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "IOR")==0){
                        material.ior = stof(tokens[1]);
                    }
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "MIRR")==0){
                        material.isMirr = bool(stoi(tokens[1]));
                    }
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "TRAN")==0){
                        material.isTran = bool(stoi(tokens[1]));
                    }
                    
                    matDict[matName] = material;
                }

                else if(strcmp(tokens[0].c_str(), "NODE")==0){
                    // create node
                    // add to scene
                    string nodeName = tokens[1];
                    mat4 transform = mat4(1.0f);
                    vec3 translate, rotate, scale, center, color;
                    string parentName;
                    
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "TRANSLATION")==0){
                        translate = vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                        transform = glm::translate(transform, translate);
                    }

                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "ROTATION")==0){
                        rotate = vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                        transform = glm::rotate(transform, rotate.z, vec3(0,0,1));
                        transform = glm::rotate(transform, rotate.y, vec3(0,1,0));
                        transform = glm::rotate(transform, rotate.x, vec3(1,0,0));
                    }
                    
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "SCALE")==0){
                        scale = vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                        transform = glm::scale(transform, scale);
                    }
                    
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "CENTER")==0){
                        center = vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                    }
                    
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "PARENT")==0){
                        parentName = tokens[1];
                    }
                    
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "SHAPE")==0){
                        if (strcmp(tokens[1].c_str(), "null")==0){
                            geometry = NULL;
                        }
                        else if (strcmp(tokens[1].c_str(), "cube")==0){
                            geometry = new Cube();
                            geomTypes["CUBE"] = true;
                        }
                        else if (strcmp(tokens[1].c_str(), "sphere")==0){
                            geometry = new Sphere();
                            geomTypes["SPHERE"] = true;
                        }
                        else if (strcmp(tokens[1].c_str(), "cylinder")==0){
                            geometry = new Cylinder();
                            geomTypes["CYLINDER"] = true;
                        }
                        else if (strcmp(tokens[1].c_str(), "mesh")==0){
                            geometry = new Mesh();
                            utilityCore::safeGetline(inFilePointer, line);
                            cout<<line<<endl;
                            tokens = utilityCore::tokenizeString(line);
                            if (strcmp(tokens[0].c_str(), "FILE")==0){
                                string objFilePath = "scenes/" + tokens[1];
                                geometry->parseObj(objFilePath);
                            }
                        }
                    }
                    
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "MAT")==0){
                        if (strcmp(tokens[1].c_str(), "null")!=0) {
                            string nodeMatName = tokens[1].c_str();
                            geometry->setMaterial(matDict[nodeMatName]);
                        }
                    }
                    else if (strcmp(tokens[0].c_str(), "RGBA")==0){
                        if (strcmp(tokens[1].c_str(), "null")!=0) {
                            color = vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
                            geometry->setColor(color);
                        }
                    }
                    
                    if (parentName == "null"){
                        Node* newNode = new Node(geometry, nodeName);
                        newNode->setTransformation(transform);
                        newNode->setColor(color);
                        this->addNode(newNode);
                    }
                    else{
                        auto index = std::distance(nodes.begin(),
                                                   std::find_if(nodes.begin(),
                                                                nodes.end(),
                                                                [parentName](Node* obj) {return obj->getNodeName() == parentName;}));
                        Node* newNode = new Node(nodes.at(index), geometry, nodeName);
                        newNode->setTransformation(transform);
                        newNode->setColor(color);
                        this->addNode(newNode);
                    }
                }
                else if (strcmp(tokens[0].c_str(), "OUTPUT")==0){
                    outFilePath = tokens[1];
                }
                if (outFilePath=="") outFilePath = "render.bmp";
            }
        }
    }
    inFilePointer.close();
}

void Scene::render(){
    
    // for all pixels generate rayDir
    // check for ray-box(voxelspace) intersection
    film = new Film();
    raytracer = new Raytracer();
    
//    cout<<"[raytracer] Progressive output is currently disabled"<<endl;
    
    for (int i = 0; i < HEIGHT; i++) {
//        cout << "\r[raytracer] " << ((i+1)*100)/HEIGHT << "% completed       " << flush;
        for (int j = 0; j < WIDTH; j++) {
            uvec2 pixel = uvec2(i,j);
            Ray ray = camera->generateRay(pixel);
            vec3 color = raytracer->trace(ray, scene->maxDepth);
            film->put(pixel, color);
        }
    }
    film->writeImage(outFilePath);
    cout<<"[raytracer] Render Successful. Output Image Path - "<<outFilePath<<endl;
}

void Scene::addNode(Node* node)
{
    if(node != NULL)
    {
        this->nodes.push_back(node);
    }
}

void Scene::load()
{
    // create perpetual storage
    nodes[0]->load();
}

void Scene::draw()
{
    // call recursive draw on root node - nodes[0]
    transformations.push(glm::mat4(1.0f));
    nodes[0]->draw(transformations);
}

std::string Scene::textFileRead(const char *filename)
{
    // http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
    std::ifstream in(filename, std::ios::in);
    if (!in) {
        std::cerr << "Error reading file" << std::endl;
        throw (errno);
    }
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

void Scene::printGLErrorLog()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << error << ": ";
        const char *e =
        error == GL_INVALID_OPERATION             ? "GL_INVALID_OPERATION" :
        error == GL_INVALID_ENUM                  ? "GL_INVALID_ENUM" :
        error == GL_INVALID_VALUE                 ? "GL_INVALID_VALUE" :
        error == GL_INVALID_INDEX                 ? "GL_INVALID_INDEX" :
        "unknown";
        std::cerr << e << std::endl;
        // Throwing here allows us to use the debugger stack trace to track
        // down the error.
#ifndef __APPLE__
        // But don't do this on OS X. It might cause a premature crash.
        // http://lists.apple.com/archives/mac-opengl/2012/Jul/msg00038.html
        throw;
#endif
    }
}

void Scene::printLinkInfoLog(int prog)
{
    GLint linked;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (linked == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL LINK ERROR" << std::endl;
    
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;
    
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);
    
    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}

void Scene::printShaderInfoLog(int shader)
{
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled == GL_TRUE) {
        return;
    }
    std::cerr << "GLSL COMPILE ERROR" << std::endl;
    
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;
    
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
    
    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
        std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
        delete[] infoLog;
    }
    // Throwing here allows us to use the debugger to track down the error.
    throw;
}

void Scene::initShader(){
    // Read in the shader program source files

    std::string vertSourceS = textFileRead("shaders/vert.glsl");
    std::string fragSourceS = textFileRead("shaders/frag.glsl");

    const char *vertSource = vertSourceS.c_str();
    const char *fragSource = fragSourceS.c_str();
    
    // Tell the GPU to create new shaders and a shader program
    GLuint shadVert = glCreateShader(GL_VERTEX_SHADER);
    GLuint shadFrag = glCreateShader(GL_FRAGMENT_SHADER);
    shaderProgram = glCreateProgram();
    
    // Load and compiler each shader program
    // Then check to make sure the shaders complied correctly
    
    // - Vertex shader
    glShaderSource    (shadVert, 1, &vertSource, NULL);
    glCompileShader   (shadVert);
    printShaderInfoLog(shadVert);
    // - Diffuse fragment shader
    glShaderSource    (shadFrag, 1, &fragSource, NULL);
    glCompileShader   (shadFrag);
    printShaderInfoLog(shadFrag);
    
    // Link the shader programs together from compiled bits
    glAttachShader  (shaderProgram, shadVert);
    glAttachShader  (shaderProgram, shadFrag);
    glLinkProgram   (shaderProgram);
    printLinkInfoLog(shaderProgram);
    
    // Clean up the shaders now that they are linked
    glDetachShader(shaderProgram, shadVert);
    glDetachShader(shaderProgram, shadFrag);
    glDeleteShader(shadVert);
    glDeleteShader(shadFrag);
    
    // Find out what the GLSL locations are, since we can't pre-define these
    locationPos    = glGetAttribLocation (shaderProgram, "vs_Position");
    locationNor    = glGetAttribLocation (shaderProgram, "vs_Normal");
    locationCol    = glGetAttribLocation (shaderProgram, "vs_Color");
    unifViewProj   = glGetUniformLocation(shaderProgram, "u_ViewProj");
    unifModel      = glGetUniformLocation(shaderProgram, "u_Model");
    unifModelInvTr = glGetUniformLocation(shaderProgram, "u_ModelInvTr");
    unifLightPos   = glGetUniformLocation(shaderProgram, "u_LightPos");
    unifLightColor = glGetUniformLocation(shaderProgram, "u_LightColor");
    unifCamPos     = glGetUniformLocation(shaderProgram, "u_CamPos");
    
    //printGLErrorLog();
}

void Scene::cleanup()
{
    glDeleteProgram(scene->shaderProgram);
    delete this->geometry;
}

Node::Node(string nodeName)
{
    parent = NULL;
    geometry = NULL;
}

Node::Node(Geometry* geometry, string nodeName):geometry(geometry), nodeName(nodeName)
{
    parent = NULL;
}

Node::Node(Node* parent, Geometry* geometry, string nodeName):parent(parent), geometry(geometry), nodeName(nodeName)
{
    if(parent != NULL)
        parent->addChildNode(this);
}

Node::~Node(void)
{
    parent = NULL;
    children.clear();
}

void Node::setParentNode(Node* NewParent)
{
    if(parent != NULL)
    {
        parent->removeChildNode(this);
    }
    parent = NewParent;
}

void Node::addChildNode(Node* ChildNode)
{
    if(ChildNode != NULL)
    {
        if(ChildNode->getParentNode() != NULL)
        {
            ChildNode->setParentNode(this);
        }
        children.push_back(ChildNode);
    }
}

void Node::removeChildNode(Node* ChildNode)
{
    if(ChildNode != NULL && !children.empty())
    {
        for(GLuint i = 0; i < children.size(); ++i)
        {
            if(children[i] == ChildNode)
            {
                children.erase(children.begin() + i);
            }
        }
    }
}

void Node::Update(int type, float n)
{
    if(!children.empty())
    {
        for(GLuint i = 0; i < children.size(); ++i)
        {
            if(children[i] != NULL)
            {
                children[i]->Update(type, n);
            }
        }
    }
}

void Node::load(){
    if (this->geometry != NULL) {
//        cout<<this->nodeName<<endl;
        this->geometry->load();
    }
    if (this->children.size() > 0){
        for(int i=0; i < children.size(); i++){
            children.at(i)->load();
        }
    }
}

void Node::draw(stack<mat4> transformations){
    
    transformations.push(transformations.top()*this->transformation);

    /*******************/
    // PREORDER TRAVERSAL//
    /*******************/
    
    // draw node
    // Tell the GPU which shader program to use to draw things
    glUseProgram(scene->shaderProgram);
    
    // Take a close look at how vertex, normal, color, and index informations
    // are created and uploaded to the GPU for drawing. You will need to do
    // something similar to get your scene graph to draw.
    glm::mat4 model = transformations.top();

    // Set the 4x4 model transformation matrices
    // Pointer to the first element of the array
    glUniformMatrix4fv(scene->unifModel, 1, GL_FALSE, &model[0][0]);
    
    //     Also upload the inverse transpose for normal transformation
    const glm::mat4 modelInvTranspose = glm::inverse(glm::transpose(model));
    glUniformMatrix4fv(scene->unifModelInvTr, 1, GL_FALSE, &modelInvTranspose[0][0]);

    glm::mat4 view = glm::lookAt(scene->camera->eye, scene->camera->center, scene->camera->up);
    glm::mat4 projection = glm::perspective<float>(scene->camera->fovy, (float)scene->getWidth()/scene->getHeight(), 0.1f, 100.0f);
    glm::mat4 perspective = projection * view;
    glUniformMatrix4fv(scene->unifViewProj, 1, GL_FALSE, &perspective[0][0]);
    
    glUniform3fv(scene->unifLightPos, 1, &scene->lightPos[0]);
    glUniform3fv(scene->unifLightColor, 1, &scene->lightColor[0]);
    
    // bind the VAO
    if(this->geometry != NULL)
    {
    glBindVertexArray(this->geometry->vao);
    const GLuint numIndices = this->geometry->getIndexCount();
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
        
    // unbind the VAO
    glBindVertexArray(0);
    // unbind the program
    glUseProgram(0);
    }
        
    if (this->children.size() == 0) {
        // a leaf node
        return;
    }
    else if (this->children.size() > 0){
        for(int i=0; i < children.size(); i++){
            children.at(i)->draw(transformations);
        }
    }
    transformations.pop();
}

Intersect Node::intersect(stack<mat4> &transformations, const Ray &r){
    
    transformations.push(transformations.top()*this->transformation);
    glm::mat4 model = transformations.top();
    Intersect isx;
    if (this->geometry != NULL)
        isx = this->geometry->intersect(model, r);
    
    if (this->children.size() > 0){
        for(int i=0; i < children.size(); i++){
            Intersect childIsx = children.at(i)->intersect(transformations, r);
            if (isx.t==-1 || (childIsx.t < isx.t)){
                if (childIsx.t>=0) isx = childIsx;
            }
        }
    }
    transformations.pop();
    return isx;
}

const bool Node::isRootNode(void) const
{
    return (parent == NULL);
}

const bool Node::isLeafNode(void) const
{
    return (children.size() == 0);
}

Film::Film(){
    pixels = vector<vector<vec3>>(scene->getHeight(), vector<vec3>(scene->getWidth(), vec3(0, 0, 0)));
}

vec3 Film::get(uvec2 pixel){
    return pixels[pixel.x][pixel.y];
}

void Film::put(uvec2 pixel, vec3 color){
    pixels[pixel.x][pixel.y] += color;
}

void Film::writeImage(string path){
    int bpp = 24;
    
    BMP output;
    output.SetSize(scene->getWidth(), scene->getHeight());
    output.SetBitDepth(bpp);
    
    for(unsigned int i = 0; i < scene->getHeight(); i++) {
        for(unsigned int j = 0; j < scene->getWidth(); j++) {
            vec3 color  = pixels[i][j];
            output(j, i)->Red = clamp(color[0], 0.0f, 1.0f)*255;
            output(j, i)->Green = clamp(color[1], 0.0f, 1.0f)*255;
            output(j, i)->Blue = clamp(color[2], 0.0f, 1.0f)*255;
        }
    }
    output.WriteToFile(path.c_str());
}