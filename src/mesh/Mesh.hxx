#ifndef MESH_HXX_
#define MESH_HXX_

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include <vector>


class Mesh {
  public:
    /* Vector of vertices defining the mesh */
    std::vector<GLfloat> vertices;
    /* Vector of normals defining the mesh */
    std::vector<GLfloat> normals;
    /* Vector of indices defining the faces of the mesh */
    std::vector<GLuint> indices;

    /* ID of the vertex buffer */
    GLuint vertexBufferId;
    /* ID of the normal buffer */
    GLuint normalBufferId;
    /* ID of the indices buffer */
    GLuint indexBufferId;

    /* Mass of the mesh */
    GLfloat mass = 1;

    /* Origin of the mesh */
    sf::Vector3f origin = {0.0, 0.0, 0.0};

    /* Constructor */
    explicit Mesh();

    /* Initialization of the buffer objects, must be called after filling
      vectors of vertices, normals and indices */
    void initBuffers();

    /* Draw the mesh in the 3D scene */
    void draw();

    /* Destructor, this will remove the buffers from memory */
    ~Mesh();
};

#endif
