#include "utils.hxx"

#include <GL/gl.h>

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>

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

std::vector<GLfloat> getPerspectiveProjMatrix(
    GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar){
  GLfloat f = 1.0/tan(fovy/2.0);

  std::vector<GLfloat> matrix = {
    f/aspect, 0, 0, 0,
    0, f, 0, 0,
    0, 0, (zNear + zFar)/(zNear - zFar), -1,
    0, 0, 2 * zNear * zFar/(zNear - zFar), 0
  };

  return matrix;
};

std::vector<GLfloat> getOrthoProjMatrix(
  GLfloat left, GLfloat right,
  GLfloat bottom, GLfloat top,
  GLfloat nearVal, GLfloat farVal){
  std::vector<GLfloat> matrix = {
    2/(right - left), 0, 0, 0,
    0, 2/(top - bottom), 0, 0,
    0, 0, -2/(farVal - nearVal), 0,
    (left + right)/(left - right), (bottom + top)/(bottom - top),
      (nearVal + farVal)/(nearVal - farVal), 1
  };

  return matrix;
};

void normalize(GLfloat* vector){
  GLfloat norm = sqrt(
    pow(vector[0], 2) + pow(vector[1], 2) + pow(vector[2], 2)
  );

  vector[0] /= norm;
  vector[1] /= norm;
  vector[2] /= norm;
};

void cross(GLfloat* vector1, GLfloat* vector2, GLfloat* result){
  result[0] = vector1[1] * vector2[2] - vector2[1] * vector1[2];
  result[1] = vector1[2] * vector2[0] - vector2[2] * vector1[0];
  result[2] = vector1[0] * vector2[1] - vector2[0] * vector1[1];
};

std::vector<GLfloat> getLookAtMatrix(
  GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ,
  GLfloat centerX, GLfloat centerY, GLfloat centerZ,
  GLfloat upX, GLfloat upY, GLfloat upZ){
  // Get forward vector (center - eye)
  GLfloat forward[3] = {
    centerX - eyeX,
    centerY - eyeY,
    centerZ - eyeZ
  };

  GLfloat up[3] = {upX, upY, upZ};

  normalize(forward);

  // Get side vector (forward x up)
  GLfloat side[3];
  cross(forward, up, side);
  normalize(side);

  // Compute the real up (side x forward)
  cross(side, forward, up);
  normalize(up);

  // Compute translation factors
  GLfloat tx = - side[0] * eyeX - side[1] * eyeY - side[2] * eyeZ + 1;
  GLfloat ty = - up[0] * eyeX - up[1] * eyeY - up[2] * eyeZ + 1;
  GLfloat tz = forward[0] * eyeX + forward[1] * eyeY + forward[2] * eyeZ + 1;

  std::vector<GLfloat> matrix = {
    side[0], up[0], -forward[0], 0,
    side[1], up[1], -forward[1], 0,
    side[2], up[2], -forward[2], 0,
    tx, ty, tz, 1
  };

  return matrix;
};
