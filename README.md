=================
Pathtracer
=================
Created by Debanshu Singh
12/08/2014

=================
USAGE
=================
Open VS project
Input scene file as cmd line parameter - default is set to ${PROJECT_DIR}/scenes/scene.txt

Extra parameter for scene file to enable monte carlo
MONTECARLO 1

=================
Features
=================
5.1 OpenMP - implemented in Scene::render function
5.2 Anti-aliasing - 4x Jittered Super-Sampling
5.3 Area Lights & Soft Shadows
	----------------------
	|Performance Analysis| 
	----------------------
	Scene - ${PROJECT_DIR}/scenes/dragon_arealights.txt
	Image - dragon_arealights.bmp
	With    OpenMP - 10.3s
	Without OpenMP - 36.2s

6.0 Cross Platform Compatible. thirdparty folder includes libraries for building on OSX/WINDOWS

6.1 [Monte Carlo] (EXTRA CREDIT) ray.cpp
	- Indirect Illumination
	----------------------
	|Performance Analysis| 
	----------------------
	Scene - ${PROJECT_DIR}/scenes/montecarlo.txt
	Image - montecarlo_cornell.bmp
	With 200 iterations - 320s
	
6.3 [Acceleration] (EXTRA CREDIT) bvh.h
    - Bounding Volume Hierarchy (BVH)
    - AABB top-down construction
    - Median Cut Partitioning with Rotating Axis Splits (based on this paper - http://gamma.cs.unc.edu/BVH/paper_rt06.pdf)
    - maximum tree depth - height increases until 2 leaves remain
    - spanning triangles - add to both left and right
	----------------------
	|Performance Analysis| 
	----------------------
	Scene - ${PROJECT_DIR}/scenes/dragon.txt
	Image - dragon.bmp
	With    Acceleration(&OpenMP) - 10.3s
	Without Acceleration(&OpenMP) - >2 hours (does not finish)
	
=================
RAYTRACER DESIGN	
=================
class Raytracer
	- implements trace function which contains soft-shadows and monte carlo (EXTRA CREDIT) 
class Scene
    - parse scene files
    - build scene graph as a tree with nodes of Class Node
class Node
	- objects of type node are stored in the Scene Graph
	- provides accessors for children and parent
class Geometry 
	- added functions load and setColor 
class Mesh 
    - added BVH creation (EXTRA CREDIT)
class Camera
    - provides camera information for calculating lookAt

=================
External Sources 
=================
UtilityCore from Yining Karl Li's TAKUA render - http://www.yiningkarlli.com for Safegetline
Design based on Raytracer Design Note - http://inst.eecs.berkeley.edu/~cs184/fa09/resources/raytracing.htm

