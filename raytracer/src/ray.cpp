//
//  ray.cpp
//  raytracer
//
//  Created by Debanshu on 11/5/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#include "scene.h"
#include "Intersect.h"
#include "sampling.h"

vec3 Raytracer::trace(Ray &r, int depth){
    
    vec3 blackColor = vec3(0,0,0);
    
    if (depth == 0) return blackColor;
    
    stack<mat4> transformations;
	transformations.push(glm::mat4(1.0f));

	Intersect isx = scene->nodes[0]->intersect(r);

	vec3 isxPos = r.pos + isx.t*r.dir;
	vec3 reflRayDir = glm::normalize((r.dir - 2*glm::dot(r.dir,isx.normal)*isx.normal));
	vec3 viewerDir = glm::normalize(scene->camera->eye - isxPos);

	vec3 lightDir = glm::normalize(scene->getLightPos() - isxPos);

	vec3 reflRayOrigin = isxPos + 0.01f*isx.normal;
	Ray reflRay = Ray(reflRayOrigin, reflRayDir);

    if (!isx.hit){
        return blackColor; // return black // check later for parallel to light case
    }
    else {
		
        if (!scene->isMonteCarlo){
			// ordinary raytracing with arealights + soft shadows
			vec3 returnColor(0,0,0);

			if (isx.geom->material.isEmit) {
				return isx.geom->material.diffColor;
			}
			
			if (isx.geom->material.isMirr) {
            returnColor += isx.geom->material.specColor*trace(reflRay, depth-1);
			}

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
					refr = trace(refrRay, depth-1);
				}

			}
			returnColor += refr;

            vec3 blinnPhongColor(0,0,0);
            const int maxShadowRays = 10;
			// increase for higher shadow quality from area lights
            for (int i=0; i<maxShadowRays; i++){
                vec3 lightPos = scene->getLightPos();

				if (!inShadow(isxPos, lightPos, isx.geom)){
                    lightDir = glm::normalize(lightPos - isxPos);
                    vec3 halfAngle = glm::normalize(lightDir + viewerDir);

                    if (!isx.geom->material.isTran) {
                        // Diffuse
						
                        blinnPhongColor += scene->lightColor*
                            isx.geom->material.diffColor*
                            glm::clamp(glm::dot(isx.normal,lightDir),0.0f,1.0f);

                        // Specular
                        if (!isx.geom->material.isMirr) {
                            blinnPhongColor += scene->lightColor*
                                isx.geom->material.specColor*0.6f*
                                pow(glm::clamp(glm::dot(isx.normal,halfAngle),0.0f,1.0f),isx.geom->material.specExpo);

                        }
                    }
                }
            }
            returnColor += blinnPhongColor*(1/(float)maxShadowRays);
			return returnColor;
        }

        else {
            // Monte-Carlo Raytracing with caustics
			if (isx.geom->material.isEmit) {
				return isx.geom->material.diffColor*isx.geom->material.emittance*r.transmittance;
			}

			if (isx.geom->material.isTran) {
				vec3 refr = blackColor;
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
					return trace(reflRay, depth); // Total Internal Refl
				else {

					vec3 refrRayDir = glm::normalize(eta*I - (eta*nDotI + sqrt(rootTerm))*isx.normal);
					vec3 refrRayOrigin = isxPos-0.01f*isx.normal;
					vec3 refrRayTransmittance = r.transmittance*vec3(1,1,1);
					Ray refrRay = Ray(refrRayOrigin, refrRayDir, !r.inside, refrRayTransmittance);
					return trace(refrRay, depth);
				}
			}
			
			if (isx.geom->material.isMirr) {
				reflRay.transmittance = r.transmittance*isx.geom->material.specColor;
				return isx.geom->material.specColor*trace(reflRay, depth-1);
			}

			if (!(isx.geom->material.isEmit || isx.geom->material.isMirr || isx.geom->material.isTran)){
			
				vec3 diffColor = isx.geom->material.diffColor;
				float absorbance = 1 - std::max(diffColor[0],std::max(diffColor[1], diffColor[2]));
				if ((float)rand()/(float)RAND_MAX < absorbance) return blackColor; // absorb the ray

				vec3 diffRayOrigin = reflRayOrigin;
				vec3 diffRayDir = glm::normalize(getCosineWeightedDirection(isx.normal));
				vec3 diffRayTransmittance = r.transmittance * diffColor;
				Ray diffRay = Ray(diffRayOrigin, diffRayDir, diffRayTransmittance);
				return trace(diffRay, depth-1);
			}
		}
    }
    return blackColor;
}

bool Raytracer::inShadow(vec3 isxPos, vec3 lightPos, Geometry* geom){
    vec3 shadowRayDir = glm::normalize(lightPos-isxPos);
    float maxT = glm::length(lightPos-isxPos);
    vec3 shadowRayOrigin = isxPos + 0.001f*shadowRayDir;

    Ray shadowRay = Ray(shadowRayOrigin, shadowRayDir);
    
    Intersect shadowIsx = scene->nodes[0]->intersect(shadowRay);
	
    if (!shadowIsx.hit) {
        return false;
    }
    else {
		if (shadowIsx.geom->material.isEmit) return false;
		if (shadowIsx.geom == geom) return false;

		vec3 shadowIsxPos = shadowRayOrigin + shadowIsx.t*shadowRayDir;
	    float shadowIsxDist = glm::length(shadowIsxPos-shadowRayOrigin);
        if (shadowIsxDist>maxT+EPSILON) {
            return false;
        }
        else return true;
    }
}