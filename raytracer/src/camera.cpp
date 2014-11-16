//
//  camera.cpp
//  raytracer
//
//  Created by Debanshu on 11/8/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#include "scene.h"

void Camera::setFrame(){
    
    w = glm::normalize(center-eye);
    u = glm::normalize(glm::cross(w, up));
    v = glm::normalize(glm::cross(u, w));
}

Ray Camera::generateRay(uvec2 pixel){
    
    float alpha, beta;
    
    alpha = tan(fovy*0.5f*PI/180) * aspectRatio * ((pixel[1] - (scene->getWidth()/2.0f)) / (scene->getWidth()/2.0f));
    beta  = tan(fovy*0.5f*PI/180) * (((scene->getHeight()/2.0f) - pixel[0]) / (scene->getHeight()/2.0f));
    
    vec3 rayDir = alpha*u + beta*v + w;
    rayDir = glm::normalize(rayDir);
    
    return Ray(eye, rayDir);
}
