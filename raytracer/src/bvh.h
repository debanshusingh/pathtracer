//
//  bvh.h
//  raytracer
//
//  Created by Debanshu on 11/20/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#ifndef raytracer_bvh_h
#define raytracer_bvh_h

#include "Geometry.h"

class BVH;
class BVHNode;


struct BVHPrimitiveInfo{
    BVHPrimitiveInfo(int pn, const BBox &b):primitiveNumber(pn), bounds(b){
        centroid = 0.5f*(b.bBoxMin + b.bBoxMax);
    }
    int primitiveNumber;
    glm::vec3 centroid;
    BBox bounds;
};

struct LinearBVHNode{
    BBox bounds;
    union {
        uint32_t primitivesOffset;  //leaf
        uint32_t secondChildOffset; //interior
    };
    uint8_t nPrimitives;            // 0 => interior node
    uint8_t axis;                   // interior node partition axis - xyz
    uint8_t pad[2];                 // ensure 32 byte total size
};

class BVH:public Geometry{
public:
    BVH(const std::vector<Geometry*> &p, uint32_t maxPrims=1, const string &sM ="sah");
    
    uint32_t maxPrimsInNode;
    enum SplitMethod {SPLIT_MIDDLE, SPLIT_EQUAL_COUNTS, SPLIT_SAH};
    SplitMethod splitMethod;
    std::vector<Geometry*> primitives;
    std::vector<Geometry*> orderedPrims;
    std::vector<BVHPrimitiveInfo> buildData;
    uint32_t totalNodes;
    BVHNode* root;
    LinearBVHNode* nodes;

    BVHNode* recursiveBuild(vector<BVHPrimitiveInfo> &buildData, uint32_t start, uint32_t end, uint32_t *totalNodes, vector<Geometry*> &orderedPrims);
    uint32_t flattenBVHTree(BVHNode* node, uint32_t *offset);
    Intersect intersectImpl(const Ray &r) const;
};


class BVHNode{
public:
    BBox bounds;
    // binary BVH
    BVHNode* left;
    BVHNode* right;

    uint32_t splitAxis, firstPrimOffset, nPrimitives;
    
    BVHNode(){left = right = NULL;}
    void initLeaf(uint32_t first, uint32_t n, const BBox &b);
    void initInterior(uint32_t axis, BVHNode* lChild, BVHNode* rChild);

};


//
//typedef vector<Geometry *>::iterator node_itr;
//
//BBox combineBoundingBoxes(vector<Geometry *> &triangleList) {
//    node_itr i = triangleList.begin();
//    node_itr end = triangleList.end();
//    BBox b = (*i)->bbox;
//    i ++;
//    for (; i != end; i ++) {
//        b = b.combine((*i)->bbox);
//    }
//    return b;
//}
//
//void partition(vector<Geometry *> &nodes, vec3 mid, int axis,
//               vector<Geometry *> &left, vector<Geometry *> &right) {
//    node_itr i = nodes.begin();
//    node_itr end = nodes.end();
//    for (; i != end; i ++) {
//        if ((*i)->bbox.bBoxMax[axis] < mid[axis])
//            left.push_back(*i);
//        else
//            right.push_back(*i);
//    }
//    if (left.size() == nodes.size()) {
//        right.push_back(left.back());
//        left.pop_back();
//    }
//    else if (right.size() == nodes.size()) {
//        left.push_back(right.back());
//        right.pop_back();
//    }
//}
//
//BVHNode* createBVH(vector<Geometry* > &triangleList, int axis) {
//    int n = (int)triangleList.size();
//
//    if (n == 0) {
//        return NULL;
//    } else if (n == 1) {
//        return new BVHNode(triangleList[0]->bbox, triangleList[0], NULL);
//    } else if (n == 2) {
//        BBox b = triangleList[0]->bbox.combine(triangleList[1]->bbox);
//        return new BVHNode(b, triangleList[0], triangleList[1]);
//    } else {
//        BBox b = combineBoundingBoxes(triangleList);
//        vec3 mid = b.midpoint();
//        vector<Geometry *> leftList, rightList;
//        partition(triangleList, mid, axis, leftList, rightList);
//        BVHNode *leftTree, *rightTree;
//        leftTree = createBVH(leftList, (axis + 1) % 3);
//        rightTree = createBVH(rightList, (axis + 1) % 3);
////        utilityCore::printVec3(b.bBoxMax);
//        return new BVHNode(b, leftTree, rightTree);
//    }
//}
//


#endif
