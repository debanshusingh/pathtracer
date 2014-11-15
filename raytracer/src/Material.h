//
//  Material.h
//  raytracer
//
//  Created by Debanshu on 11/14/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#ifndef __raytracer__Material__
#define __raytracer__Material__

#include "scene.h"

class Material
{
public:
    vec3 diffColor, specColor;
    float specExpo;
    bool mirr;
};

#endif /* defined(__raytracer__Material__) */
