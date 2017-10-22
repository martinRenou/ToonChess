#ifndef MESH_HXX_
#define MESH_HXX_

#include <GL/gl.h>

#include <vector>
#include <string>

class Mesh {
  public:
    std::string filePath;

    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLuint> indices;

    GLuint vertexBufferId;
    GLuint normalBufferId;
    GLuint indexBufferId;

    Mesh(std::string filePath);

    void initBuffers();
    void draw();
    void clearBuffers();
};

#endif
