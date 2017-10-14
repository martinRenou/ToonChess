#include "Mesh.hxx"

template<typename Out>
void split(const std::string &s, char delim, Out result){
  std::stringstream ss;
  ss.str(s);
  std::string item;

  while(std::getline(ss, item, delim)){
    *(result++) = item;
  }
}

std::vector<std::string> split(const std::string &s, char delim){
  std::vector<std::string> elems;
  split(s, delim, std::back_inserter(elems));
  return elems;
}

void extractFloatVec3(std::vector<std::string> *line, std::vector<GLfloat> *vector){
  for(int i = 1; i <=3; i++){
    vector->push_back(
      std::stof(line->at(i))
    );
  }
}

void extractFace(std::vector<std::string> *line, std::vector<GLuint> *indices){
  for(int i = 1; i <= 3; i++){
    indices->push_back(
      std::stoi(split(line->at(i), '/').at(0)) - 1
    );
  }
}

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
