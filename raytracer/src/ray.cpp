//
//  ray.cpp
//  raytracer
//
//  Created by Debanshu on 11/5/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#include "scene.h"
#include "Intersect.h"

vec3 Raytracer::trace(const Ray &r, int depth){
    
    vec3 blackColor = vec3(0,0,0);
    
    if (depth == 0) return blackColor;
    
    scene->transformations.push(glm::mat4(1.0f));
    Intersect isx = scene->nodes[0]->intersect(scene->transformations, r);
    vec3 returnColor(0,0,0);

    if (!isx.hit){
        return blackColor; // return black // check later for parallel to light case
    }
    else {
        vec3 isxPos = r.pos + isx.t*r.dir;
        
        vec3 refr = blackColor;
        if (isx.geom->material.isTran) {
            float ior1 = r.ior;
            float ior2 = isx.geom->material.ior;
            
            if(ior1 != 1.0f) { // flip the ior
                isx.normal *= -1.0f;
                ior2 = 1.0f;
            }
            
            float ior12 = ior1/ior2;
            vec3 I = r.dir;
            float nDotI = glm::dot(isx.normal,-I);
            
            float rootTerm = 1 - pow(ior12,2)*(1-pow(nDotI,2));
            if (rootTerm<0) return refr; // Total Internal Refl
            else {
                vec3 refrRayDir = glm::normalize((-ior12*nDotI - sqrt(rootTerm))*isx.normal + ior12*I);
                vec3 refrRayOrigin = isxPos+0.01f*refrRayDir;
                Ray refrRay = Ray(refrRayOrigin, refrRayDir);
                refr = trace(refrRay, depth-1);
            }
            
        }
        returnColor += refr;
        
        if (!inShadow(isxPos, scene->lightPos)){
            vec3 reflDir = glm::normalize((r.dir - 2*glm::dot(r.dir,isx.normal)*isx.normal));
            vec3 lightDir = glm::normalize(scene->lightPos - isxPos);
            vec3 viewerDir = glm::normalize(scene->camera->eye - isxPos);
            vec3 halfAngle = glm::normalize(lightDir+viewerDir);
            
            returnColor += scene->lightColor*(isx.geom->material.diffColor*glm::dot(isx.normal,lightDir));
            
            vec3 spec = isx.geom->material.specColor;
            if (isx.geom->material.isMirr) {
                Ray reflRay = Ray(isxPos, reflDir);
                spec = trace(reflRay, depth-1);
            }
            returnColor += scene->lightColor*(spec*pow(std::max(glm::dot(reflDir,halfAngle),0.0f),isx.geom->material.specExpo));

        }
        else return blackColor;
    }
    
    for (int i=0; i<3; i++) {
        returnColor[i] = utilityCore::clamp(returnColor[i], 0, 1);
    }

    return returnColor;
}

bool Raytracer::inShadow(vec3 isxPos, vec3 lightPos){
    vec3 shadowRayDir = glm::normalize(lightPos-isxPos);
    float maxT = glm::length(lightPos-isxPos);
    vec3 shadowRayOrigin = isxPos + 0.01f*shadowRayDir;

    Ray shadowRay = Ray(shadowRayOrigin, shadowRayDir);
    Intersect shadowIsx = scene->nodes[0]->intersect(scene->transformations, shadowRay);
    vec3 shadowIsxPos = shadowRayOrigin + shadowIsx.t*shadowRayDir;
    float shadowIsxDist = glm::length(shadowIsxPos-shadowRayOrigin);
    
    if (!shadowIsx.hit) {
        return false;
    }
    else {
        if (shadowIsxDist>maxT) {
            return false;
        }
        else return true;
    }
}