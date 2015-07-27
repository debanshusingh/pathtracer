#include "Geometry.h"

Geometry::Geometry(geometryType geomType) :
    type_(geomType)
{
}

 Geometry::~Geometry()
{
    vertices_.clear();
    normals_.clear();
    colors_.clear();
    indices_.clear();
//    glDeleteBuffers(1, &this->vbo);
    glDeleteBuffers(1, &this->vboPos);
    glDeleteBuffers(1, &this->vboCol);
    glDeleteBuffers(1, &this->vboNor);
    glDeleteBuffers(1, &this->vboIdx);
    glDeleteBuffers(1, &this->vao);
}

void Geometry::load(){
    // Modern OpenGL requires VAOs
    
    vector<glm::vec3> vertices = this->getVertices();
    vector<glm::vec3> colors = this->getColors();
    vector<glm::vec3> normals = this->getNormals();
    vector<GLuint> indices = this->getIndices();

    // bind the VBO and VAO
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);
    
    glGenBuffers(1, &this->vboPos);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboPos);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(scene->locationPos);
    glVertexAttribPointer(scene->locationPos, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//    glVertexAttribPointer(scene->locationPos, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), NULL);
    
    // Bind+upload the color data
    glGenBuffers(1, &this->vboCol);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboCol);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(scene->locationCol);
    glVertexAttribPointer(scene->locationCol, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//    glVertexAttribPointer(scene->locationCol, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

    // Bind+upload the normal data
    glGenBuffers(1, &this->vboNor);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboNor);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(scene->locationNor);
    glVertexAttribPointer(scene->locationNor, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//    glVertexAttribPointer(scene->locationNor, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (const GLvoid*)(6 * sizeof(GLfloat)));
    
    // Bind+upload the indices to the GL_ELEMENT_ARRAY_BUFFER.
    glGenBuffers(1, &this->vboIdx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIdx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    
    // unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Geometry::setColor(vec3 color){
    // colors
    for (GLuint i = 0; i < vertices_.size(); ++i) {
        colors_.push_back(color);
    }
}

void Geometry::setMaterial(const Material &mat){
    this->setColor(mat.diffColor);
    this->material = mat;
}

Intersect Geometry::intersect(const glm::mat4 &T, Ray ray_world)
{
    // The input ray here is in WORLD-space. It may not be normalized!
    
    // normalize ray_world.
    glm::mat4 T_inv = glm::inverse(T);
    
    // Transform the ray into OBJECT-LOCAL-space, for intersection calculation.
    Ray ray_local;
    ray_local.dir = vec3(T_inv*vec4(glm::normalize(ray_world.dir), 0));
    ray_local.pos = vec3(T_inv*vec4(ray_world.pos, 1));
    
    // transform the ray by the inverse transformation.
    //     (Remember that position = vec4(vec3, 1) while direction = vec4(vec3, 0).)
    
    // Compute the intersection in LOCAL-space.
    Intersect isx = intersectImpl(ray_local);
    
    if (isx.t != -1) {
        // Transform the local-space intersection BACK into world-space.
        //     (Note that, as long as you didn't re-normalize the ray direction
        //     earlier, `t` doesn't need to change.)
        const glm::vec3 normal_local = isx.normal;
        glm::vec3 normal_world;
        normal_world = vec3(glm::transpose(T_inv)*vec4(normal_local,0));
        // inverse-transpose-transform the normal to get it back from local-space to world-space.
        //     (If you were transforming a position, you would just use the unmodified transform T.)
        //     http://www.arcsynthesis.org/gltut/Illumination/Tut09%20Normal%20Transformation.html
        if (glm::sign(glm::dot(normal_world, ray_world.dir)) >= 0) normal_world *= -1.0f;
        isx.normal = glm::normalize(normal_world);
        isx.hit = true;
        isx.geom = this;
    }
    
    // The final output intersection data is in WORLD-space.
    return isx;
}

BBox::BBox(vec3 m, vec3 M) {
    for (int i = 0; i < 3; i ++) {
        bBoxMin[i] = std::min(m[i], M[i]);
        bBoxMax[i] = std::max(m[i], M[i]);
    }
}



