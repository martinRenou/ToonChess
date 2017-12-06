#include "utils.hxx"

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

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
  GLfloat f = 1.0/tan(fovy * M_PI/360.);

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

void normalize(sf::Vector3f* vector){
  GLfloat norm = sqrt(
    pow(vector->x, 2) + pow(vector->y, 2) + pow(vector->z, 2)
  );

  vector->x /= norm;
  vector->y /= norm;
  vector->z /= norm;
};

void cross(sf::Vector3f vector1, sf::Vector3f vector2, sf::Vector3f* result){
  result->x = vector1.y * vector2.z - vector2.y * vector1.z;
  result->y = vector1.z * vector2.x - vector2.z * vector1.x;
  result->z = vector1.x * vector2.y - vector2.x * vector1.y;
};

std::vector<GLfloat> getLookAtMatrix(
    sf::Vector3f eye, sf::Vector3f center, sf::Vector3f up){
  // Get forward vector (center - eye)
  sf::Vector3f forward = {
    center.x - eye.x,
    center.y - eye.y,
    center.z - eye.z
  };

  normalize(&forward);

  // Get side vector (forward x up)
  sf::Vector3f side;
  cross(forward, up, &side);
  normalize(&side);

  // Compute the real up (side x forward)
  cross(side, forward, &up);
  normalize(&up);

  // Compute translation factors
  GLfloat tx = - side.x * eye.x - side.y * eye.y - side.z * eye.z + 1;
  GLfloat ty = - up.x * eye.x - up.y * eye.y - up.z * eye.z + 1;
  GLfloat tz = forward.x * eye.x + forward.y * eye.y + forward.z * eye.z + 1;

  std::vector<GLfloat> matrix = {
    side.x, up.x, -forward.x, 0,
    side.y, up.y, -forward.y, 0,
    side.z, up.z, -forward.z, 0,
    tx, ty, tz, 1
  };

  return matrix;
};

std::vector<GLfloat> getIdentityMatrix(){
  std::vector<GLfloat> matrix = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };

  return matrix;
};

std::vector<GLfloat> matrixProduct(
    std::vector<GLfloat>* matrix1, std::vector<GLfloat>* matrix2){
  std::vector<GLfloat> result = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
  };

  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      result.at(4 * i + j) =
        matrix1->at(4 * i + 0)*matrix2->at(4 * 0 + j) +
        matrix1->at(4 * i + 1)*matrix2->at(4 * 1 + j) +
        matrix1->at(4 * i + 2)*matrix2->at(4 * 2 + j) +
        matrix1->at(4 * i + 3)*matrix2->at(4 * 3 + j);
    }
  }

  return result;
};

std::vector<GLfloat> rotate(
    std::vector<GLfloat>* matrix,
    GLfloat angle,
    sf::Vector3f r){
  GLfloat co = cos(angle * M_PI/180.);
  GLfloat si = sin(angle * M_PI/180.);
  normalize(&r);

  GLfloat a = r.x * r.x * (1 - co) + co;
  GLfloat b = r.x * r.y * (1 - co) - r.z * si;
  GLfloat c = r.x * r.z * (1 - co) + r.y * si;

  GLfloat d = r.y * r.x * (1 - co) + r.z * si;
  GLfloat e = r.y * r.y * (1 - co) + co;
  GLfloat f = r.y * r.z * (1 - co) - r.x * si;

  GLfloat g = r.z * r.x * (1 - co) - r.y * si;
  GLfloat h = r.z * r.y * (1 - co) + r.x * si;
  GLfloat i = r.z * r.z * (1 - co) + co;

  std::vector<GLfloat> rotationMatrix = {
    a, d, g, 0,
    b, e, h, 0,
    c, f, i, 0,
    0, 0, 0, 1
  };

  std::vector<GLfloat> result = matrixProduct(matrix, &rotationMatrix);

  return result;
};

std::vector<GLfloat> translate(
    std::vector<GLfloat>* matrix, sf::Vector3f translation){
  std::vector<GLfloat> translationMatrix = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    translation.x, translation.y, translation.z, 1
  };

  std::vector<GLfloat> result = matrixProduct(matrix, &translationMatrix);

  return result;
};

std::vector<GLfloat> inverse(std::vector<GLfloat>* matrix){
  GLfloat det;
  std::vector<GLfloat> inv = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
  };
  std::vector<GLfloat> m = *matrix;

  inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] +
    m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];

  inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] -
    m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];

  inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] +
    m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];

  inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] -
    m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

  inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] -
    m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];

  inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] +
    m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];

  inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] -
    m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];

  inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] +
    m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

  inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] +
    m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];

  inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] -
    m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];

  inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] +
    m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];

  inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] -
    m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

  inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] -
    m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];

  inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] +
    m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];

  inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] -
    m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];

  inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] +
    m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

  // Compute determinant
  det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

  if (det == 0){
    std::cerr << "Matrix not inversible" << '\n';;
  }

  det = 1.0 / det;

  for(int i = 0; i < 16; i++){
    inv[i] = inv[i] * det;
  }

  return inv;
};
