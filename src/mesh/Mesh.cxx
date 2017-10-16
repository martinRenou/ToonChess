#include "Mesh.hxx"

Mesh::Mesh(std::string filePath){
  this->filePath = filePath;
}

void Mesh::load(){
  // Read obj file
  std::ifstream fobj(this->filePath);
  std::string line;

  this->vertices.clear();
  this->normals.clear();
  this->indices.clear();

  while(std::getline(fobj, line)){
    std::vector<std::string> splittedLine = split(line, ' ');

    if(splittedLine.at(0).compare("v") == 0){
      extractFloatVec3(&splittedLine, &this->vertices);
      continue;
    }

    if(splittedLine.at(0).compare("vn") == 0){
      extractFloatVec3(&splittedLine, &this->normals);
      continue;
    }

    if(splittedLine.at(0).compare("f") == 0){
      extractFace(&splittedLine, &this->indices);
      continue;
    }
  }

  return;
}
