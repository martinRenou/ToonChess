#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>

#include <string>
#include <vector>
#include <fstream>

#include "Mesh.hxx"
#include "../utils/utils.hxx"


void extractFloatVec3(
    std::vector<std::string> *line, std::vector<GLfloat> *vector){
  for(int i = 1; i <= 3; i++){
    vector->push_back(
      std::stof(line->at(i))
    );
  }
}

void extractVertices(
    std::vector<std::string> *line,
    std::vector<GLfloat> *unsortedVertices,
    std::vector<GLfloat> *vertices){
  for(int i = 1; i <= 3; i++){
    int vertexIndex = std::stoi(split(line->at(i), '/').at(0)) - 1;

    for(int j = 0; j <= 2; j++){
      vertices->push_back(unsortedVertices->at(3 * vertexIndex + j));
    }
  }
}

void extractNormals(
    std::vector<std::string> *line,
    std::vector<GLfloat> *unsortedNormals,
    std::vector<GLfloat> *normals){
  for(int i = 1; i <= 3; i++){
    int normalIndex = std::stoi(split(line->at(i), '/').at(2)) - 1;

    for(int j = 0; j <= 2; j++){
      normals->push_back(unsortedNormals->at(3 * normalIndex + j));
    }
  }
}

Mesh::Mesh(const std::string& filePath) : filePath{filePath}{}

void Mesh::initBuffers(){
  // Read obj file
  std::ifstream fobj(filePath);
  std::string line;

  vertices.clear();
  normals.clear();
  indices.clear();

  std::vector<GLfloat> unsortedVertices;
  std::vector<GLfloat> unsortedNormals;

  while(std::getline(fobj, line)){
    if(line.size() == 0) continue;

    std::vector<std::string> splittedLine = split(line, ' ');

    if(splittedLine.at(0).compare("v") == 0){
      extractFloatVec3(&splittedLine, &unsortedVertices);
      continue;
    }

    if(splittedLine.at(0).compare("vn") == 0){
      extractFloatVec3(&splittedLine, &unsortedNormals);
      continue;
    }

    if(splittedLine.at(0).compare("f") == 0){
      extractVertices(&splittedLine, &unsortedVertices, &vertices);
      extractNormals(&splittedLine, &unsortedNormals, &normals);

      for(int i = 0; i <= 2; i++){
        indices.push_back(indices.size());
      }
      continue;
    }
  }

  // Vertex buffer
  glGenBuffers(1, &vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glBufferData(
    GL_ARRAY_BUFFER,
    vertices.size()*sizeof(GLfloat),
    vertices.data(),
    GL_STATIC_DRAW);

  // Normal buffer
  glGenBuffers(1, &normalBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
  glBufferData(
    GL_ARRAY_BUFFER,
    normals.size()*sizeof(GLfloat),
    normals.data(),
    GL_STATIC_DRAW);

  // Index buffer
  glGenBuffers(1, &indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    indices.size()*sizeof(GLuint),
    indices.data(),
    GL_STATIC_DRAW);

  // Unbind buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return;
}

void Mesh::draw(){
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  // Send vertices
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glVertexPointer(
    3,
    GL_FLOAT,
    0,
    (void*)0
  );

  // Send normals
  glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
  glNormalPointer(
    GL_FLOAT,
    0,
    (void*)0
  );

  // Draw triangles
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
  glDrawElements(
    GL_TRIANGLES,
    indices.size(),
    GL_UNSIGNED_INT,
    (void*)0
  );

  // Unbind buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
}

Mesh::~Mesh(){
  glDeleteBuffers(1, &vertexBufferId);
  glDeleteBuffers(1, &normalBufferId);
  glDeleteBuffers(1, &indexBufferId);
}
