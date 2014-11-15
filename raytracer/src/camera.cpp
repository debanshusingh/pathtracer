//
//  camera.cpp
//  raytracer
//
//  Created by Debanshu on 11/8/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#include "scene.h"

Ray Camera::generateRay(uvec2 pixel){
    
    C = glm::normalize(center-eye); // C
    A = glm::normalize(glm::cross(C, up)); // A = C x U
    B = glm::normalize(glm::cross(A, C)); // B = A x C
    
    float dist = glm::length(center);
    float aspectRatio = (float)scene->getWidth()/scene->getHeight();
    float fovx = 2*atan(aspectRatio*tan(fovy/2));
    
    vec3 V = B*dist*tan(fovy/2);
    vec3 H = A*dist*tan(fovx/2);
    
    float sx = (float)pixel.y/(scene->getWidth()-1);
    float sy =(float)pixel.x/(scene->getHeight()-1);
    
    vec3 rayDir = C + (2*sx - 1)*H - (2*sy - 1)*V;  // ndc2world
    rayDir = glm::normalize(rayDir);
    
    return Ray(eye, rayDir);
}
