#define GL_GLEXT_PROTOTYPES

#include <GL/glu.h>

#include <string>
#include <vector>
#include <fstream>

#include "Mesh.hxx"
#include "../utils.hxx"

Mesh::Mesh(std::string filePath){
  this->filePath = filePath;
}

void Mesh::initBuffers(){
  // Read obj file
  std::ifstream fobj(this->filePath);
  std::string line;

  this->vertices.clear();
  this->normals.clear();
  this->indices.clear();

  std::vector<GLfloat> unsortedNormals;

  while(std::getline(fobj, line)){
    if(line.size() == 0) continue;

    std::vector<std::string> splittedLine = split(line, ' ');

    if(splittedLine.at(0).compare("v") == 0){
      extractFloatVec3(&splittedLine, &this->vertices);
      continue;
    }

    if(splittedLine.at(0).compare("vn") == 0){
      extractFloatVec3(&splittedLine, &unsortedNormals);
      continue;
    }

    if(splittedLine.at(0).compare("f") == 0){
      extractFace(&splittedLine, &this->indices);
      extractNormal(&splittedLine, &unsortedNormals, &this->normals);
      continue;
    }
  }

  // Vertex buffer
  glGenBuffers(1, &this->vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferId);
  glBufferData(
    GL_ARRAY_BUFFER,
    this->vertices.size()*sizeof(GLfloat),
    this->vertices.data(),
    GL_STATIC_DRAW);

  // Normal buffer
  glGenBuffers(1, &this->normalBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, this->normalBufferId);
  glBufferData(
    GL_ARRAY_BUFFER,
    this->normals.size()*sizeof(GLfloat),
    this->normals.data(),
    GL_STATIC_DRAW);

  // Index buffer
  glGenBuffers(1, &this->indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBufferId);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    this->indices.size()*sizeof(GLuint),
    this->indices.data(),
    GL_STATIC_DRAW);

  // Unbind buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return;
}

void Mesh::draw(){
  glEnableClientState(GL_VERTEX_ARRAY);

  // Send vertices
  glBindBuffer(GL_ARRAY_BUFFER, this->vertexBufferId);
  glVertexPointer(
    3,
    GL_FLOAT,
    0,
    (void*)0
  );

  // Send normals
  glBindBuffer(GL_ARRAY_BUFFER, this->normalBufferId);
  glNormalPointer(
    GL_FLOAT,
    0,
    (void*)0
  );

  // Draw triangles
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBufferId);
  glDrawElements(
    GL_TRIANGLES,
    this->indices.size(),
    GL_UNSIGNED_INT,
    (void*)0
  );

  // Unbind buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDisableClientState(GL_VERTEX_ARRAY);
}

Mesh::~Mesh(){
  glDeleteBuffers(1, &this->vertexBufferId);
  glDeleteBuffers(1, &this->normalBufferId);
  glDeleteBuffers(1, &this->indexBufferId);
}
