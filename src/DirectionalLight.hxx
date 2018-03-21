#ifndef DIRECTIONALLIGHT_HXX_
#define DIRECTIONALLIGHT_HXX_

#include <vector>

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

struct DirectionalLight {
  // Light direction should NOT be collinear to (0, 0, 1) to prevent
  // lightLookAtMatrix computation issues, and should NOT be collinear to
  // (1, 1, 0) to prevent shadows artefacts
  sf::Vector3f direction = {-1.0, 1.0, -1.0};
  std::vector<GLfloat> viewMatrix;
  std::vector<GLfloat> projectionMatrix;
};

#endif
