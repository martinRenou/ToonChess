#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include "loadObjFile.hxx"

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

std::vector<Mesh *> loadObjFile(const std::string& filePath){
  // Read obj file
  std::ifstream fobj(filePath);
  std::string line;

  std::vector<GLfloat> unsortedVertices;
  std::vector<GLfloat> unsortedNormals;

  std::vector<Mesh*> meshes;
  Mesh* currentMesh;

  while(std::getline(fobj, line)){
    if(line.size() == 0) continue;

    std::vector<std::string> splittedLine = split(line, ' ');

    // The line starts with an "o", it's a new mesh definition
    if(splittedLine.at(0).compare("o") == 0){
      currentMesh = new Mesh();
      meshes.push_back(currentMesh);

      continue;
    }

    // The line starts with an "m", it's the mesh's mass
    if(splittedLine.at(0).compare("mass") == 0){
      currentMesh->mass = std::stof(splittedLine.at(1));

      continue;
    }

    // The line starts with "origin", it's the mesh's origin
    if(splittedLine.at(0).compare("origin") == 0){
      currentMesh->origin = sf::Vector3f(
        std::stof(splittedLine.at(1)),
        std::stof(splittedLine.at(2)),
        std::stof(splittedLine.at(3))
      );

      continue;
    }

    // The line starts with a "v", it's a new vertex
    if(splittedLine.at(0).compare("v") == 0){
      extractFloatVec3(&splittedLine, &unsortedVertices);
      continue;
    }

    // The line starts with a "vn", it's a vertex normal
    if(splittedLine.at(0).compare("vn") == 0){
      extractFloatVec3(&splittedLine, &unsortedNormals);
      continue;
    }

    // The line starts with an "f", it's a triangle definition
    if(splittedLine.at(0).compare("f") == 0){
      extractVertices(&splittedLine, &unsortedVertices, &currentMesh->vertices);
      extractNormals(&splittedLine, &unsortedNormals, &currentMesh->normals);

      for(int i = 0; i <= 2; i++){
        currentMesh->indices.push_back(currentMesh->indices.size());
      }
      continue;
    }
  }

  for(unsigned int i = 0; i < meshes.size(); i++)
    meshes.at(i)->initBuffers();

  return meshes;
};
