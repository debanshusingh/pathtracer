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
#include "bvh.h"
#include "sampling.h"
#include <iomanip>

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

                    //  enable preview lighting

                }

                else if(strcmp(tokens[0].c_str(), "MONTECARLO")==0){
                    scene->isMonteCarlo = bool(stoi(tokens[1]));
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
                    utilityCore::safeGetline(inFilePointer, line);
                    cout<<line<<endl;
                    tokens = utilityCore::tokenizeString(line);
                    if (strcmp(tokens[0].c_str(), "EMIT")==0){
                        material.emittance = stof(tokens[1]);
                        if (material.emittance > 0) material.isEmit = true;
                        else material.isEmit = false;
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
                            Mesh* mesh = new Mesh();
                            geometry = static_cast<Geometry*>(mesh);
                            utilityCore::safeGetline(inFilePointer, line);
                            cout<<line<<endl;
                            tokens = utilityCore::tokenizeString(line);
                            
                            if (strcmp(tokens[0].c_str(), "FILE")==0){
                                string objFilePath = "scenes/" + tokens[1];
                                parseObj(mesh, objFilePath, mesh->triangleList);
                            }
                            cout<<"[raytracer] Building BVH acceleration structure"<<endl;
                            mesh->tree = new BVH(mesh->triangleList,1,"middle");
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
    // check for ray-box intersection
    clock_t timeStart = clock();
    film = new Film();
    raytracer = new Raytracer();
    updateGlobalTransform(nodes);
	
	int percentComplete = 0;
	
	//increase/decrease sampleCount appropriately for anti-aliasing & monte-carlo
	int sampleCount;
	if (isMonteCarlo) sampleCount = 100; //monte-carlo iterations
	else sampleCount = 1; //anti-aliasing supersampling
	
	//====================//
	// NEW LESSON LEARNED //
	//====================//
	// How I solved the multithreading race condition and slow recursion? - 
	// Slow recursion was being caused by the shared transformStack
	// Thought about the variable being shared and how it slowed recursion - traversing scenegraph every intersection was inefficient
	// Solved by pre-processing global transformation.
	// IMPACT - increased speed +10X and removed race condition 
	
	#pragma omp parallel for
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            vec3 pixelColor(0,0,0);
            for (int k =0; k<sampleCount; k++){
				// jittered supersampling
                float randJitter = (float) rand()/RAND_MAX;
                vec2 pixel = vec2(i+randJitter, j+randJitter);
                Ray ray = camera->generateRay(pixel);
                vec3 returnColor = raytracer->trace(ray, scene->maxDepth);
				pixelColor += returnColor;
            }
            pixelColor /= sampleCount;
            pixelColor = glm::clamp(pixelColor, vec3(0.0f), vec3(1.0f));
            uvec2 pixel = uvec2(i,j);
            film->put(pixel, pixelColor);
        }
	
		#pragma omp critical(percentComplete)
		percentComplete++;
		if (percentComplete % 11 == 10) cout << "\r[raytracer] " << ((percentComplete)*100)/(float)HEIGHT << "% completed       " << flush;
    }

    film->writeImage(outFilePath);
    clock_t timeEnd = clock();
    cout<<"[raytracer] Render Successful. Output Image Path - "<<outFilePath<<endl;
    cout<<"[raytracer] Render time - "<< setprecision(5) << (float)(timeEnd - timeStart) / CLOCKS_PER_SEC << " (sec)\n"<<endl;
    
}

vec3 Scene::getLightPos(){
	// assume 1 area light in scene
	// lightNode contains that area light
	string lightName = "light";
	auto index = std::distance(nodes.begin(),
                                                   std::find_if(nodes.begin(),
                                                                nodes.end(),
                                                                [lightName](Node* obj) {return obj->getNodeName() == lightName;}));

	Node* areaLightNode = nodes.at(index);
	
	if (areaLightNode->getGeometry()->getGeometryType() == Geometry::geometryType::CUBE){
		vec3 lightPos = getRandomPointOnCube(areaLightNode);
		scene->lightColor = areaLightNode->getGeometry()->material.diffColor;
		return lightPos;
	}
    else {
		vec3 lightPos = getRandomPointOnSphere(areaLightNode);
		scene->lightColor = areaLightNode->getGeometry()->material.diffColor;
		return lightPos;
	}
		
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

void Node::load(){
    if (this->geometry != NULL) {
        this->geometry->load();
    }
    if (this->children.size() > 0){
        for(int i=0; i < children.size(); i++){
            children.at(i)->load();
        }
    }
}

void Node::draw(stack<mat4> transformations){
    
    transformations.push(transformations.top()*this->localTransformation);

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

void Scene::updateGlobalTransform(vector<Node*> nodes){
	typedef vector<Node *>::iterator node_itr;
	node_itr i = nodes.begin();
    node_itr end = nodes.end();
	for(;i != end; i ++) {
		(*i)->globalTransformation = (*i)->getGlobalTransformation();
	}
}

Intersect Node::intersect(const Ray &r){
    
	glm::mat4 model = this->globalTransformation;
    Intersect isx;
    if (this->geometry != NULL)
        isx = this->geometry->intersect(model, r);
    
    if (this->children.size() > 0){
        for(int i=0; i < children.size(); i++){
            Intersect childIsx = children.at(i)->intersect(r);
            if (isx.t==-1 || (childIsx.t < isx.t)){
                if (childIsx.t>=0) isx = childIsx;
            }
        }
    }
    return isx;
}

mat4 Node::getGlobalTransformation(){
    if (this->parent == NULL) {
        return this->localTransformation;
    }
    else return this->localTransformation*this->getParentNode()->getGlobalTransformation();
}

const bool Node::isRootNode(void) const
{
    return (parent == NULL);
}

const bool Node::isLeafNode(void) const
{
    return (children.size() == 0);
}

void Scene::parseObj(Mesh* mesh, const string &inFilePath, vector<Geometry*> &triangleList){
    
//    if code reaches here, it means we have a mesh
    
    vector<glm::vec3> temp_vertices;        // vertex buffer
    vector<GLuint> temp_indices;            // index buffer
    vector<glm::vec3> temp_vertexNormals;   // normal buffer
    
    ifstream objFilePointer;
    objFilePointer.open(inFilePath.c_str());
    if (objFilePointer.is_open()){
        cout<<"[raytracer] Reading OBJ from "<< inFilePath <<endl;
        while (objFilePointer.good()) {
            string line;
            utilityCore::safeGetline(objFilePointer, line);
            if ((line.find_first_not_of(" \t\r\n") != string::npos) && (line[0] != '#')) {
                vector<string> tokens = utilityCore::tokenizeString(line);
                if(strcmp(tokens[0].c_str(), "v")==0){
                    temp_vertices.push_back(vec3(stof(tokens[1]), stof(tokens[2]), stof(tokens[3])));
                    temp_vertexNormals.push_back(vec3(0,0,0));
                }
                else if(strcmp(tokens[0].c_str(), "f")==0){
                    unsigned int vertexIndex[3];
                    for (int i=1; i<=3; i++) {
                        string s;
                        istringstream f(tokens[i]);
                        getline(f, s, '/');
                        
                        GLuint trueIndex = static_cast<GLuint>(stoi(s)-1); // -1 to account for OBJ offset of +1
                        vertexIndex[i-1] = trueIndex;
                        temp_indices.push_back(trueIndex);
                        mesh->vertices_.push_back(temp_vertices[trueIndex]);
                    }
                    vec3 faceNormal = glm::cross((temp_vertices[vertexIndex[1]] - temp_vertices[vertexIndex[0]]),
                                                 (temp_vertices[vertexIndex[2]] - temp_vertices[vertexIndex[0]]));
                    // add face_normal contribution to vertexNormal in a std:map and store trueindices in a vector
                    for (int i=0; i<3; i++){
                        mesh->normals_.push_back(faceNormal); // add facenormal value to each vertex
                        temp_vertexNormals.at(vertexIndex[i]) += faceNormal;
                    }
                }
            }
        }
    }
    objFilePointer.close();

    for ( int i=0; i<mesh->vertices_.size(); i++) {
        //        normals_.at(i) = glm::normalize(normals_.at(i)); // flat shading
        mesh->normals_.at(i) = glm::normalize(temp_vertexNormals.at(temp_indices.at(i))); // smooth shading
        mesh->indices_.push_back(i);
    }

    unsigned int trianglesInMesh = mesh->getIndexCount()/3;
    for (int i=0; i<trianglesInMesh; i++) {
        vec3 v0 = mesh->vertices_.at(mesh->indices_.at(3*i));
        vec3 v1 = mesh->vertices_.at(mesh->indices_.at(3*i+1));
        vec3 v2 = mesh->vertices_.at(mesh->indices_.at(3*i+2));
        
        vec3 boxMin, boxMax;
        for(int i=0; i<3; i++){
            boxMin[i] = std::min(v0[i], std::min(v1[i],v2[i]));
            boxMax[i] = std::max(v0[i], std::max(v1[i],v2[i]));
        }
        BBox triBbox = BBox(boxMin, boxMax);
        Triangle* tri = new Triangle(v0, v1, v2);
        tri->bbox = triBbox;

        triangleList.push_back(tri);
    }
    
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