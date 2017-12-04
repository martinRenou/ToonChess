#include "utils.hxx"

#include <GL/gl.h>

#include <sstream>
#include <fstream>
#include <iostream>
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

bool _displayGLErrors(const char *file, int line){
  GLenum errorCode;
  bool foundError(false);

  while((errorCode = glGetError()) != GL_NO_ERROR){
    std::string error = "UNKNOWN_ERROR";

    switch(errorCode) {
      case GL_INVALID_OPERATION:
        error = "GL_INVALID_OPERATION";
        break;
      case GL_INVALID_ENUM:
        error = "GL_INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        error = "GL_INVALID_VALUE";
        break;
      case GL_OUT_OF_MEMORY:
        error = "GL_OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        error = "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
      case GL_STACK_UNDERFLOW:
        error = "GL_STACK_UNDERFLOW";
        break;
      case GL_STACK_OVERFLOW:
        error = "GL_STACK_OVERFLOW";
        break;
    }

    std::cerr << file << ":" << line << " -> Error code: "
      << errorCode << "(" << error.c_str() << ")"  << std::endl;
    foundError = true;
  }

  return foundError;
}

std::vector<GLdouble> getOrthoProjMatrix(
  GLdouble left, GLdouble right,
  GLdouble bottom, GLdouble top,
  GLdouble nearVal, GLdouble farVal){
  std::vector<GLdouble> matrix = {
    2/(right - left), 0, 0, 0,
    0, 2/(top - bottom), 0, 0,
    0, 0, -2/(farVal - nearVal), 0,
    (left + right)/(left - right), (bottom + top)/(bottom - top),
      (nearVal + farVal)/(nearVal - farVal), 1
  };

  return matrix;
};
