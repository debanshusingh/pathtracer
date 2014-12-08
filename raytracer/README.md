===========
Scene Graph
===========

Assignment HW3c
Created by Debanshu Singh
12/08/2014
=================
External Sources 
UtilityCore from Yining Karl Li's TAKUA render - http://www.yiningkarlli.com for Safegetline
Design based on Raytracer Design Note - http://inst.eecs.berkeley.edu/~cs184/fa09/resources/raytracing.htm
=================

=================
Usage
Open VS project
Input scene file as cmd line parameter - default is set to ${PROJECT_DIR}/scenes/scene.txt
=================

==============================
CONSIDERATION FOR EXTRA CREDIT
==============================
- Cross Platform Compatible. thirdparty folder includes libraries for building on OSX/WINDOWS

6.3 [Acceleration] 
    - Bounding Volume Hierarchy (BVH)
    - AABB top-down construction
    - Median Cut Partitioning with Rotating Axis Splits (based on this paper - http://gamma.cs.unc.edu/BVH/paper_rt06.pdf)
    - maximum tree depth - 2 leaves at end
    - spanning triangles - choose by centroid of bbox

----------------------
|Performance Analysis| 
----------------------
Dragon Scene - ${PROJECT_DIR}/scenes/dragon.txt

With    Acceleration - 7.4s
Without Acceleration - >10 mins (does not finish in less than 10 mins)

=================
raytracer Features
=================
5.1 OpenMP
5.2 Anti-aliasing - 4x Super-Sampling
5.3 Area Lights & Soft Shadows

class Scene
    - parse scene files (completes OBJ loader requirement)
    - build scene graph as a tree with nodes of Class Node
class Node
	- objects of type node are stored in the Scene Graph
	- provides accessors for children and parent
class Geometry 
	- added functions load and setColor 
class Mesh 
    - added per requirement (completes Mesh requirement)
class Camera
    - provides camera information for calculating lookAt

frag.glsl
vert.glsl (shader augmentation requirement)
KEYPRESS requirements (implemented differently because of GLFW shift modifier bug)
F/V = +- 0.5 in X
G/B = +- 0.5 in Y
H/N = +- 0.5 in Z