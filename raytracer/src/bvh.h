//
//  bvh.h
//  raytracer
//
//  Created by Debanshu on 11/20/14.
//  Copyright (c) 2014 Debanshu. All rights reserved.
//

#ifndef raytracer_bvh_h
#define raytracer_bvh_h

#include "Intersect.h"

typedef vector<Geometry *>::iterator node_itr;

BBox combineBoundingBoxes(vector<Geometry *> &triangleList) {
    node_itr i = triangleList.begin();
    node_itr end = triangleList.end();
    BBox b = (*i)->bbox;
    i ++;
    for (; i != end; i ++) {
        b = b.combine((*i)->bbox);
    }
    return b;
}

void partition(vector<Geometry *> &nodes, vec3 mid, int axis,
               vector<Geometry *> &left, vector<Geometry *> &right) {
    node_itr i = nodes.begin();
    node_itr end = nodes.end();
    for (; i != end; i ++) {
        if ((*i)->bbox.bBoxMax[axis] < mid[axis])
            left.push_back(*i);
        else
            right.push_back(*i);
        // recursion doesn't converge - why? - think about tree height
//        else if ((*i)->bbox.bBoxMin[axis] < mid[axis] && (*i)->bbox.bBoxMax[axis] > mid[axis]){
//            left.push_back(*i);
//            right.push_back(*i);
//        }
    }
    if (left.size() == nodes.size()) {
        right.push_back(left.back());
        left.pop_back();
    }
    else if (right.size() == nodes.size()) {
        left.push_back(right.back());
        right.pop_back();
    }
}
BVHNode* createBVH(vector<Geometry* > &triangleList, int axis) {
    int n = (int)triangleList.size();

    if (n == 0) {
        return NULL;
    } else if (n == 1) {
        return new BVHNode(triangleList[0]->bbox, triangleList[0], NULL);
    } else if (n == 2) {
        BBox b = triangleList[0]->bbox.combine(triangleList[1]->bbox);
        return new BVHNode(b, triangleList[0], triangleList[1]);
    } else {
        BBox b = combineBoundingBoxes(triangleList);
        vec3 mid = b.midpoint();
        vector<Geometry *> leftList, rightList;
        partition(triangleList, mid, axis, leftList, rightList);
        BVHNode *leftTree, *rightTree;
        leftTree = createBVH(leftList, (axis + 1) % 3);
        rightTree = createBVH(rightList, (axis + 1) % 3);
//        utilityCore::printVec3(b.bBoxMax);
        return new BVHNode(b, leftTree, rightTree);
    }
}

Intersect BVHNode::intersectImpl(const Ray &r) const{
    if (!bbox.isHit(r))
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

#endif
