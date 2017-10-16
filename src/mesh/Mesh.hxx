#ifndef MESH_HXX_
#define MESH_HXX_

#include <GL/gl.h>

#include <vector>
#include <string>

class Mesh {
  public:
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLuint> indices;

    std::string filePath;

    Mesh(std::string filePath);

    void load();
};

#endif
