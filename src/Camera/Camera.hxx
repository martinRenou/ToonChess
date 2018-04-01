#ifndef CAMERA_HXX_
#define CAMERA_HXX_

#include <vector>

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

class Camera {
private:
  /* Camera field of view */
  GLint fovy = 50;

  /* Position of the camera in the space coordinates */
  sf::Vector3f position;

  /* Look at position */
  sf::Vector3f center = {0.0, 0.0, 0.0};

  /* Camera up vector */
  sf::Vector3f up = {0.0, 0.0, 1.0};

  /* Rotation radius of the camera */
  GLfloat radius = 40.0;

  /* Rotation angle around Z axis */
  GLfloat phi = 0.0;

  /* Rotation angle around X axis */
  GLfloat theta = 0.0;

  /* Rotation speed in radians per pixel */
  GLfloat rotationSpeed = 0.002;

  /* Camera damping */
  GLfloat dPhi = 0.0;
  GLfloat dTheta = 0.0;
  GLfloat damping = 0.9;

  void checkConstraints();
  void computeViewMatrix();

public:
  /* Constructor */
  explicit Camera(GLfloat screenRatio);

  /* Update the camera perspective matrix */
  void updatePerspective(GLfloat screenRatio);

  /* Move camera, given the mouse movement on the screen */
  void move(GLfloat dX, GLfloat dY, GLfloat screenRatio);

  /* Update the camera position using damping */
  void update();

  /* Camera view matrix */
  std::vector<GLfloat> viewMatrix;

  /* Camera projection matrix */
  std::vector<GLfloat> projectionMatrix;

  /* Destructor */
  ~Camera(){};
};

#endif
