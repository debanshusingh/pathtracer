//
//  bvh.cpp
//  raytracer
//
//  Created by Debanshu on 7/25/15.
//  Copyright (c) 2015 Debanshu. All rights reserved.
//

#include "bvh.h"

BBox BBox::combine(const BBox &b){
    vec3 m, M;
    for (int i = 0; i < 3; i ++) {
        m[i] = std::min(this->bBoxMin[i], b.bBoxMin[i]);
        M[i] = std::max(this->bBoxMax[i], b.bBoxMax[i]);
    }
    return BBox(m, M);
}

BBox BBox::combine(const vec3 &p){
    vec3 m, M;
    for (int i = 0; i < 3; i ++) {
        m[i] = std::min(this->bBoxMin[i], p[i]);
        M[i] = std::max(this->bBoxMax[i], p[i]);
    }
    return BBox(m, M);
}

bool BBox::isHit(Ray ray) const{
    
    float tnear = - numeric_limits<float>::max();
    float tfar = numeric_limits<float>::max();
    float t1,t2,temp;
    
    for(int i =0 ;i < 3; i++){
        if(ray.dir[i] == 0){
            if(ray.pos[i] < bBoxMin[i] || ray.pos[i] > bBoxMax[i])
                return false;
        }
        else{
            t1 = (bBoxMin[i] - ray.pos[i])/ray.dir[i];
            t2 = (bBoxMax[i] - ray.pos[i])/ray.dir[i];
            if(t1 > t2){
                temp = t1;
                t1 = t2;
                t2 = temp;
            }
            if(t1 > tnear)
                tnear = t1;
            if(t2 < tfar)
                tfar = t2;
            if(tnear > tfar)
                return false;
            if(tfar < 0)
                return false;
        }
    }
    return true;
}

//BVH Member Functions
BVH::BVH(const std::vector<Geometry*> &p, uint32_t maxPrims, const string &sM):primitives(p), maxPrimsInNode(maxPrims), Geometry(BVHNODE){
    
    // decide split method
    if (sM == "middle") splitMethod = SplitMethod::SPLIT_MIDDLE;
    else if (sM == "equal") splitMethod = SplitMethod::SPLIT_EQUAL_COUNTS;
    else splitMethod = SplitMethod::SPLIT_SAH;
    
    if(primitives.size() == 0){
        root = NULL; // what is nodes?
        return;
    }
    
    // <build BVH from primitives>
    
    // <initialize buildData array with BVHPrimitiveInfo per primitive>
    buildData.reserve(primitives.size());
    for (uint32_t i=0; i<primitives.size(); ++i) {
        buildData.push_back(BVHPrimitiveInfo(i,primitives[i]->bbox));
        // TODO - does bbox need to be world bounds?
    }
    
    // <build BVH recursively>
    totalNodes = 0;
    orderedPrims.reserve(primitives.size());
    root = recursiveBuild(buildData, 0, primitives.size(), &totalNodes, orderedPrims);

    primitives.swap(orderedPrims);
    
    // flattenBVH
    nodes = new LinearBVHNode[totalNodes];
    uint32_t offset = 0;
    flattenBVHTree(root, &offset);
}

BVHNode* BVH::recursiveBuild(vector<BVHPrimitiveInfo> &buildData, uint32_t start, uint32_t end, uint32_t *totalNodes, vector<Geometry*> &orderedPrims){
    (*totalNodes)++;
    BVHNode* node = new BVHNode;
    
    //<compute bounds of all primitives in this bvh node>
    BBox bbox;
    for (uint32_t i=start; i<end; ++i) {
        bbox = bbox.combine(buildData[i].bounds);
    }
    
    uint32_t nPrimitives = end - start;
    if (nPrimitives == 1) {
    //<create Leaf BVHNode>
        uint32_t firstPrimOffset = orderedPrims.size();
        for (uint32_t i=start; i<end; ++i) {
            uint32_t primNum = buildData[i].primitiveNumber;
            orderedPrims.push_back(primitives[primNum]);
        }
        node->initLeaf(firstPrimOffset, nPrimitives, bbox);
    }
    else{
    //<create interior BVHNode>
        
        //<choose split dim>
        BBox centroidBounds;
        for (uint32_t i=start; i<end; ++i) {
            centroidBounds = centroidBounds.combine(buildData[i].centroid);
        }
        int dim = centroidBounds.maximumExtent();
    
        //<partition primitives>
        uint32_t mid = (start+end)/2;
        
        if (centroidBounds.bBoxMax[dim] == centroidBounds.bBoxMin[dim]) {
            //<create Leaf BVHNode>
            uint32_t firstPrimOffset = orderedPrims.size();
            for (uint32_t i=start; i<end; ++i) {
                uint32_t primNum = buildData[i].primitiveNumber;
                orderedPrims.push_back(primitives[primNum]);
            }
            node->initLeaf(firstPrimOffset, nPrimitives, bbox);
        }
        else{
        //<partition based on splitMethod>
            switch (splitMethod) {
                case SPLIT_MIDDLE:{
                    float pMid = 0.5f*(centroidBounds.bBoxMin + centroidBounds.bBoxMax)[dim];
                    BVHPrimitiveInfo* midPtr = std::partition(&buildData[start], &buildData[end-1]+1, [dim,pMid](const BVHPrimitiveInfo &pi){return pi.centroid[dim] < pMid;});
                    mid = midPtr - &buildData[0];
                    if (mid != start && mid != end) break;
                }
                case SPLIT_EQUAL_COUNTS:{
                    mid = (start+end)/2;
                    std::nth_element(&buildData[start], &buildData[mid], &buildData[end-1]+1, [dim](const BVHPrimitiveInfo &a, const BVHPrimitiveInfo &b){return a.centroid[dim] < b.centroid[dim];});
                    break;
                }
                default:{
                    break;
                }
            }
            
            //add children to BVHNode
            node->initInterior(dim,
                               recursiveBuild(buildData, start, mid, totalNodes, orderedPrims),
                               recursiveBuild(buildData, mid, end, totalNodes, orderedPrims));
            
        }
    }
    
    return node;
}

uint32_t BVH::flattenBVHTree(BVHNode *node, uint32_t *offset){
    LinearBVHNode *linearNode = &nodes[*offset];
    linearNode->bounds = node->bounds;
    uint32_t myOffset = (*offset)++;
    if (node->nPrimitives > 0) {
        linearNode->primitivesOffset = node->firstPrimOffset;
        linearNode->nPrimitives = node->nPrimitives;
    }
    else{
        linearNode->axis = node->splitAxis;
        linearNode->nPrimitives = 0;
        flattenBVHTree(node->left, offset);
        linearNode->secondChildOffset = flattenBVHTree(node->right, offset);
    }
    return myOffset;
}


// BVHNode Member Functions

void BVHNode::initLeaf(uint32_t first, uint32_t n, const BBox &b){
    firstPrimOffset=first;
    nPrimitives=n;
    bounds=b;
}

void BVHNode::initInterior(uint32_t axis, BVHNode *lChild, BVHNode *rChild){
    left = lChild; right = rChild;
    bounds = bounds.combine(lChild->bounds);
    bounds = bounds.combine(rChild->bounds);
    splitAxis = axis;
    nPrimitives = 0;
}

Intersect BVHNode::intersectImpl(const Ray &r) const{
    if (!bounds.isHit(r))
        return Intersect();
    Intersect leftI = Intersect();
    Intersect rightI = Intersect();
    double distL, distR;
    
    if (left != NULL)
        leftI = left->intersectImpl(r);
    
    if (right != NULL)
        rightI = right->intersectImpl(r);
    
    if (leftI.hit && rightI.hit) {
        distL = (r.dir*leftI.t).length();
        distR = (r.dir*rightI.t).length();
        if (distL < distR)
            return leftI;
        else
            return rightI;
        
    } else if (leftI.hit) {
        return leftI;
        
    } else if (rightI.hit) {
        return rightI;
        
    } else {
        return Intersect();
    }
}
