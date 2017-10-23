#ifndef MESH_HXX_
#define MESH_HXX_

#include <GL/gl.h>

#include <vector>
#include <string>

class Mesh {
  public:
    /* The path of the .obj file containing the mesh */
    std::string filePath;

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

    /* Constructor
      \param filePath The path of the .obj file containing the mesh
    */
    Mesh(std::string filePath);

    /* Initialization of the buffer objects */
    void initBuffers();

    /* Draw the mesh in the 3D scene */
    void draw();

    /* Destructor, this will remove the buffers from memory */
    ~Mesh();
};

#endif
