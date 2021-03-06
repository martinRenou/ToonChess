#ifndef UTILS_HXX_
#define UTILS_HXX_

#include <vector>
#include <string>

#include <GLFW/glfw3.h>

/* Function used to load files like shader source code
  \param path The path to the file that you want to load
  \return the string containing the content of the file
*/
std::string loadFile(const std::string& path);

/* Function used to load png images
  \param path The path to the file that you want to load
  \return the texture as a Gluint
*/
GLuint loadPNGTexture(const std::string& path);

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

#endif
