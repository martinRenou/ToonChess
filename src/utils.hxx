#ifndef UTILS_HXX_
#define UTILS_HXX_

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include <vector>
#include <string>

/* Function used to load files like shader source code
  \param path The path to the file that you want to load
  \return the string containing the content of the file
*/
std::string loadFile(std::string path);

template<typename Out>
void split(const std::string &s, char delim, Out result);

/* Function used to split string into a list of strings using a delimiter
  \param s The string that you want to split
  \param delim The delimiter used to split the string
  \return a vector of substrings
*/
std::vector<std::string> split(const std::string &s, char delim);

/* Function used to display OpenGL errors
  \param file The name of current file
  \param line The line where displayGLErrors is called
  \return a boolean which is true if there is errors, false otherwise
*/
bool _displayGLErrors(const char *file, int line);

#define displayGLErrors() _displayGLErrors(__FILE__, __LINE__)

/* Generate and return a perspective matrix. Inspired from the gluPerspective
  function, but it only creates the matrix and returns it, it doesn't call
  glMultMatrix under the hood.
  \param
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
  \return The result
*/
std::vector<GLfloat> matrixProduct(
  std::vector<GLfloat>* matrix1, std::vector<GLfloat>* matrix2
);

/* Function which rotates a matrix and return the result
  \param matrix The 4x4 matrix to rotate
  \param angle The angle of the rotation in degrees
  \param r The axis of the rotation
*/
std::vector<GLfloat> rotate(
  std::vector<GLfloat>* matrix,
  GLfloat angle,
  sf::Vector3f r
);

/* Function which translates a matrix and return the result
  \param matrix The 4x4 matrix to translate
  \param translation The vector of translation
*/
std::vector<GLfloat> translate(
  std::vector<GLfloat>* matrix,
  sf::Vector3f translation
);

#endif
