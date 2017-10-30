#ifndef UTILS_HXX_
#define UTILS_HXX_

#include <GL/gl.h>

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

#endif
