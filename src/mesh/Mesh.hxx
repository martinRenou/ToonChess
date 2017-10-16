#ifndef MESH_HXX_
#define MESH_HXX_

#include <string>
#include <vector>

#include "utils.hxx"

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
