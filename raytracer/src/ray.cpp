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
        vec3 reflRayDir = glm::normalize((r.dir - 2*glm::dot(r.dir,isx.normal)*isx.normal));
        vec3 lightDir = glm::normalize(scene->lightPos - isxPos);
        vec3 viewerDir = glm::normalize(scene->camera->eye - isxPos);
        vec3 halfAngle = glm::normalize(lightDir+viewerDir);
        
        vec3 reflRayOrigin = isxPos + 0.01f*isx.normal;
        Ray reflRay = Ray(reflRayOrigin, reflRayDir);


        vec3 refr = blackColor;
        if (isx.geom->material.isTran) {
            float ior1, ior2;
            vec3 I = r.dir;
            if(!r.inside) { // going into the medium
                ior1 = 1.0f;
                ior2 = isx.geom->material.ior;
            }
            else{ // going out of the medium
                ior1 = isx.geom->material.ior;
                ior2 = 1.0f;
            }
            float eta = ior1/ior2;
            float nDotI = glm::dot(isx.normal,I);
            float rootTerm = 1 - pow(eta,2)*(1-pow(nDotI,2));

            if (rootTerm < 0)
                refr = trace(reflRay, depth-1); // Total Internal Refl
            else {

                vec3 refrRayDir = glm::normalize(eta*I - (eta*nDotI + sqrt(rootTerm))*isx.normal);
                vec3 refrRayOrigin = isxPos-0.01f*isx.normal;
                Ray refrRay = Ray(refrRayOrigin, refrRayDir, !r.inside);


                // Fresnel - Schlick Approximation
                float costhetaI = glm::dot(isx.normal,viewerDir);
                float costhetaR = glm::dot(-isx.normal,refrRayDir);
                float costheta = std::min(costhetaI, costhetaR); // max(thetaI,thetaR) => min(costhetaI,costhetaR)
                float R0 = pow((eta-1)/(eta+1),2.0f);
                float fresnel = R0 + (1-R0)*pow((1-costheta),5.0f);
                
                refr = (1-fresnel)*trace(refrRay, depth-1) + fresnel*trace(reflRay, depth-1); // double check this later
            }
            
        }
        returnColor += refr;
        
        if (!inShadow(isxPos, scene->lightPos)){

            if (!isx.geom->material.isTran) {
                // Diffuse

                returnColor += scene->lightColor*
                            isx.geom->material.diffColor*
                            glm::clamp(glm::dot(isx.normal,lightDir),0.0f,1.0f);

                // Specular
                if (!isx.geom->material.isMirr) {
                    returnColor += scene->lightColor*
                                isx.geom->material.specColor*0.6f*
                                pow(glm::clamp(glm::dot(isx.normal,halfAngle),0.0f,1.0f),isx.geom->material.specExpo);

                }
            }

        }
        if (isx.geom->material.isMirr) {
            returnColor += isx.geom->material.specColor*trace(reflRay, depth-1);
        }
    }
    
    // Clamp colors [0,1]
    returnColor = glm::clamp(returnColor, vec3(0.0f), vec3(1.0f));
    return returnColor;
}

bool Raytracer::inShadow(vec3 isxPos, vec3 lightPos){
    vec3 shadowRayDir = glm::normalize(lightPos-isxPos);
    float maxT = glm::length(lightPos-isxPos);
    vec3 shadowRayOrigin = isxPos + 0.001f*shadowRayDir;

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