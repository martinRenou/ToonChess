#include "utils.hxx"

#include <GL/gl.h>

#include <sstream>
#include <fstream>
#include <string>
#include <vector>

std::string loadFile(std::string path){
  std::ifstream in(path);
  std::string s(
    (std::istreambuf_iterator<char>(in)),
    std::istreambuf_iterator<char>()
  );
  return s;
}

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

std::string grid[8][8] = {
  "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8",
  "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8",
  "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8",
  "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8",
  "e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8",
  "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8",
  "g1", "g2", "g3", "g4", "g5", "g6", "g7", "g8",
  "h1", "h2", "h3", "h4", "h5", "h6", "h7", "h8",
};

std::vector<int> uciFormatToPosition(std::string position){
  int x(0), y(0);
  bool found(false);
  for(x = 0; x < 8 && !found; x++){
    for(y = 0; y < 8 && !found; y++){
      if(grid[x][y].compare(position) == 0){
        found = true;
      }
    }
  }

  std::vector<int> outPosition = {x - 1, y - 1};
  return outPosition;
};

std::string positionToUciFormat(int positionX, int positionY){
  return grid[positionX][positionY];
};
