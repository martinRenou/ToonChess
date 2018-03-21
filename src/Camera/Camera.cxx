#include "../utils/math.hxx"

#include "Camera.hxx"

Camera::Camera(GLfloat screenRatio) {
  position.x = 0.0;
  position.y = - radius;
  position.z = 20.0;

  viewMatrix = getLookAtMatrix(position, center, up);
  projectionMatrix = getPerspectiveProjMatrix(
    fovy, screenRatio, 1, 1000
  );
}

void Camera::update(GLfloat screenRatio){
  // Recompute perspective matrix
  projectionMatrix = getPerspectiveProjMatrix(
    fovy, screenRatio, 1, 1000
  );
}

void Camera::move(GLfloat dX, GLfloat dY, GLfloat screenRatio){
  phi -= rotationSpeed * screenRatio * dX;
  theta += rotationSpeed * dY;

  // Constraint phi between -PI/2 and PI/2
  if(phi > M_PI / 2.0) phi = M_PI / 2.0;
  else if (phi < - M_PI / 2.0) phi = - M_PI / 2.0;

  // Constraint theta between 0.0 and PI/3
  if(theta > M_PI / 3.0) theta = M_PI / 3.0;
  else if (theta < 0.0) theta = 0.0;

  // Compute camera position according to the new rotation angle
  position.x = radius * sin(phi);
  position.y = - radius * cos(phi) * cos(theta);
  position.z = 20 + radius * sin(theta);

  viewMatrix = getLookAtMatrix(position, center, up);
}
