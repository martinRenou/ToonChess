#ifndef GAMEINFO_HXX_
#define GAMEINFO_HXX_

#include <GL/gl.h>
#include <SFML/Graphics.hpp>

#include <vector>

#include "constants.hxx"

struct GameInfo {
  // Rendering informations
  GLfloat shadowMapResolution = SHADOWMAPPING_LOW;
  GLint antialiasingLevel = ANTIALIASING_LOW;
  GLint fovy = 50;
  GLint width = 1024;
  GLint height = 576;

  GLfloat cameraRotationRadius = 40.0;

  // Light direction should NOT be collinear to (0, 0, 1) to prevent
  // lightLookAtMatrix computation issues, and should NOT be collinear to
  // (1, 1, 0) to prevent shadows artefacts
  sf::Vector3f lightDirection = {-1.0, 1.0, -1.0};

  // View and Projection matrices
  sf::Vector3f cameraPosition;
  std::vector<GLfloat> cameraViewMatrix;
  std::vector<GLfloat> cameraProjectionMatrix;
  std::vector<GLfloat> lightViewMatrix;
  std::vector<GLfloat> lightProjectionMatrix;
};

#endif
