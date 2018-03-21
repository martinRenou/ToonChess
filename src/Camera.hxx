#ifndef CAMERA_HXX_
#define CAMERA_HXX_

#include <vector>

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

struct Camera {
  GLint fovy = 50;
  sf::Vector3f position;
  GLfloat radius = 40.0;
  std::vector<GLfloat> viewMatrix;
  std::vector<GLfloat> projectionMatrix;
};

#endif
