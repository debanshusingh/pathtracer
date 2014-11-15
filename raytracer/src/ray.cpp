//
//  ray.cpp
//  raytracer
//
//  Created by Debanshu on 11/5/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#include "scene.h"
#include "Intersect.h"

vec3 Raytracer::trace(const Ray &r){
    
    scene->transformations.push(glm::mat4(1.0f));
    Intersect intersect = scene->nodes[0]->intersect(scene->transformations, r);
    vec3 finalColor = intersect.normal;
    for (int i=0; i<3; i++) {
        finalColor[i] = utilityCore::clamp(finalColor[i], 0, 1);
    }

    return finalColor;
}