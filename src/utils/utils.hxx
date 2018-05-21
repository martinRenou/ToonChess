#ifndef UTILS_HXX_
#define UTILS_HXX_

#include <vector>
#include <string>

#include <GLFW/glfw3.h>

#include "../constants.hxx"
#include "math.hxx"

struct Config
{
  int mode = FULLSCREEN;
  int shadows = SHADOWMAPPING_LOW;
  int antialiasing = ANTIALIASING_HIGH;
  std::string ai = "stockfish";
  int difficulty = DIFFICULTY_EASY;
  bool show_suggested_move = true;
  Vector4f user_pieces_color = {1.0, 0.93, 0.7, 1.0};
  Vector4f user_smoke_color = {0.41, 0.37, 0.23, 1.0};
  Vector4f ai_pieces_color = {0.51, 0.08, 0.08, 1.0};
  Vector4f ai_smoke_color = {0.3, 0.12, 0.4, 1.0};
  Vector4f background_color = {1.0, 1.0, 1.0, 1.0};
  Vector4f board_color_1 = {0.7, 0.6, 0.41, 1.0};
  Vector4f board_color_2 = {1.0, 1.0, 1.0, 1.0};
  Vector4f allowed_move_color = {0.94, 0.81, 0.34, 1.0};
};

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

/* Function used to load config file
  \param path The path to the file that you want to load
  \return the config struct
*/
Config loadConfig(const std::string& path);

/* Function used to display OpenGL errors
  \param file The name of current file
  \param line The line where displayGLErrors is called
  \return a boolean which is true if there is errors, false otherwise
*/
bool _displayGLErrors(const char *file, int line);

#define displayGLErrors() _displayGLErrors(__FILE__, __LINE__)

#endif
