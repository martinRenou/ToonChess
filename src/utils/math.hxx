#ifndef MATH_HXX_
#define MATH_HXX_

#include <vector>

#include <GL/gl.h>
#include <SFML/Graphics.hpp>

/* Generate and return a perspective matrix. Inspired from the gluPerspective
  function, but it only creates the matrix and returns it, it doesn't call
  glMultMatrix under the hood.
  \param fovy The field of view
  \param aspect The ratio aspect
  \param zNear The near value for the clipplane
  \param zFar The fat value for the clipplane
  \return The perspective matrix
*/
std::vector<GLfloat> getPerspectiveProjMatrix(
  GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar
);

/* Generate and return an orthographic matrix. Inspired from the glOrtho
  function, but it only creates the matrix and returns it, it doesn't call
  glMultMatrix under the hood.
  \param left Specify the coordinate for the left vertical clipping plane
  \param right Specify the coordinate for the right vertical clipping plane
  \param bottom Specify the coordinate for the bottom horizontal clipping plane
  \param top Specify the coordinate for the top horizontal clipping plane
  \param nearVal Specify the distance to the nearer depth clipping plane.
    This value is negative if the plane is to be behind the viewer
  \param farVal Specify the distance to the farther depth clipping plane
    This value is negative if the plane is to be behind the viewer
  \return The orthographic matrix
*/
std::vector<GLfloat> getOrthoProjMatrix(
  GLfloat left, GLfloat right,
  GLfloat bottom, GLfloat top,
  GLfloat nearVal, GLfloat farVal
);

/* Generate and return a lookAt matrix. Inspired from the gluLookAt
  function, but it only creates the matrix and returns it, it doesn't call
  glMultMatrix under the hood.
  \param eye Specifies the position of the eye point
  \param center Specifies the position of the reference point
  \param up Specifies the direction of the up vector
  \return The lookAt matrix
*/
std::vector<GLfloat> getLookAtMatrix(
  sf::Vector3f eye, sf::Vector3f center, sf::Vector3f up
);

/* Function which returns the 4x4 identity matrix
  \return The 4x4 identity matrix
*/
std::vector<GLfloat> getIdentityMatrix();

/* Function which perform a matrix product between two matrices
  \param matrix1 The first 4x4 matrix
  \param matrix2 The second 4x4 matrix
  \return The result matrix
*/
std::vector<GLfloat> matrixProduct(
  std::vector<GLfloat>* matrix1, std::vector<GLfloat>* matrix2
);

/* Function which rotates a matrix and return the result
  \param matrix The 4x4 matrix to rotate
  \param angle The angle of the rotation in degrees
  \param r The axis of the rotation
  \return The result matrix
*/
std::vector<GLfloat> rotate(
  std::vector<GLfloat>* matrix,
  GLfloat angle,
  sf::Vector3f r
);

/* Function which translates a matrix and return the result
  \param matrix The 4x4 matrix to translate
  \param translation The vector of translation
  \return The result matrix
*/
std::vector<GLfloat> translate(
  std::vector<GLfloat>* matrix,
  sf::Vector3f translation
);

/* Function which computes the inverse of a matrix and returns the result
  \param matrix The 4x4 matrix of which you want the inverse
  \return The inverse matrix
*/
std::vector<GLfloat> inverse(std::vector<GLfloat>* matrix);

/* Function which computes the transpose of a matrix and returns the result
  \param matrix The 4x4 matrix of which you want the transpose
  \return The transposed matrix
*/
std::vector<GLfloat> transpose(std::vector<GLfloat>* matrix);

#endif
