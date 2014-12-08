#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

#include "scene.h"
#include "tests.h"

// Attributes
GLuint locationPos;
GLuint locationCol;
GLuint locationNor;

// Uniforms
GLuint unifModel;
GLuint unifModelInvTr;
GLuint unifViewProj;

//void resize(int, int);
void display(void);
//void mousepress(int button, int state, int x, int y);
void cleanup(void);
void GLFWCALL keypress(int,int);
Scene* scene;

int main(int argc, char** argv)
{
    if(!glfwInit())
        throw std::runtime_error("glfwInit failed");

    // open a window with GLFW
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    

    scene = new Scene();
    scene->parseScene(argv[1]);

    if(!glfwOpenWindow(scene->getWidth(), scene->getHeight(), 8, 8, 8, 8, 24, 0, GLFW_WINDOW))
        throw std::runtime_error("glfwOpenWindow failed. Can your hardware handle OpenGL 3.2?");

    glfwSetWindowTitle("raytracer");
    glfwSetKeyCallback(keypress);
    
    // initialise GLEW
    glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
    if(glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");
    
    // print out some info about the graphics drivers
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    
    // make sure OpenGL version 3.2 API is available
    if(!GLEW_VERSION_3_2)
        throw std::runtime_error("OpenGL 3.2 API is not available.");
    
    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

	scene->initShader();
    scene->load();
//    RunTests();

    // run while the window is open
    while(glfwGetWindowParam(GLFW_OPENED)){
        display();
    }

    // clean up and exit
    scene->cleanup();
    glfwTerminate();
    return 0;
}

// update the scene based on the time elapsed since last update
void GLFWCALL keypress(int key, int action) {
    if (key == GLFW_KEY_ESC && action == GLFW_PRESS)
        exit(0);
    else if(glfwGetKey('F')){
        scene->lightPos += vec3(0.5,0,0);
    }
    else if(glfwGetKey('V')){
        scene->lightPos += vec3(-0.5,0,0);
    }
    else if(glfwGetKey('G')){
        scene->lightPos += vec3(0,0.5,0);
    }
    else if(glfwGetKey('B')){
        scene->lightPos += vec3(0,-0.5,0);
    }
    else if(glfwGetKey('H')){
        scene->lightPos += vec3(0,0,0.5);
    }
    else if(glfwGetKey('N')){
        scene->lightPos += vec3(0,0,-0.5);
    }
    else if(glfwGetKey('P')){
        scene->render();
        // run unit tests
    }
    
}

void display()
{
    // Clear the screen so that we only see newly drawn images
    // Set the color which clears the screen between frames
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Create a matrix to pass to the model matrix uniform variable in the
    // vertex shader, which is used to transform the vertices in our draw call.
    // The default provided value is an identity matrix; you'll change this.
    
    //
    // Draws the scene
    scene->draw();

    // Move the rendering we just made onto the screen
    glfwSwapBuffers();

    // Check for any GL errors that have happened recently
//    printGLErrorLog();
}

//void resize(int width, int height)
//{
//    // Set viewport
//    glViewport(0, 0, width, height);
//
//    // Get camera information
//    // Add code here if you want to play with camera settings/ make camera interactive.
//    glm::mat4 projection = glm::perspective(PI / 4, width / (float) height, 0.1f, 100.0f);
//    glm::mat4 camera = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
//    projection = projection * camera;
//
//    // Upload the projection matrix, which changes only when the screen or
//    // camera changes
//    glUseProgram(shaderProgram);
//    glUniformMatrix4fv(unifViewProj, 1, GL_FALSE, &projection[0][0]);
//
////    glutPostRedisplay();
//}
