#include <sstream>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <png.h>

#include <GLFW/glfw3.h>

#include "../constants.hxx"
#include "utils.hxx"
#include "math.hxx"

std::string loadFile(const std::string& path){
  std::ifstream in(path);
  std::string s(
    (std::istreambuf_iterator<char>(in)),
    std::istreambuf_iterator<char>()
  );
  return s;
}

GLuint loadPNGTexture(const std::string& path){
  FILE* file = fopen(path.c_str(), "r");
  if (!file)
  {
    throw std::runtime_error("The file " + path + " does not exist");
  }

  unsigned char header[8];
  fread(header, 1, 8, file);
  if (png_sig_cmp(header, 0, 8))
  {
    fclose(file);
    throw std::runtime_error("The file " + path + " is not a png file");
  }

  png_structp png_ptr = png_create_read_struct(
    PNG_LIBPNG_VER_STRING, NULL, NULL, NULL
  );
  if (!png_ptr)
  {
    fclose(file);
    throw std::runtime_error("File loading failed during png_create_read_struct");
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    fclose(file);
    throw std::runtime_error("File loading failed during png_create_info_struct");
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (!end_info)
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(file);
    throw std::runtime_error("File loading failed during png_create_info_struct");
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(file);
    throw std::runtime_error("File loading failed during setjmp");
  }

  png_init_io(png_ptr, file);
  png_set_sig_bytes(png_ptr, 8);

  png_read_info(png_ptr, info_ptr);

  int bit_depth, color_type;
  png_uint_32 width, height;

  png_get_IHDR(
    png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
    NULL, NULL, NULL
  );

  GLint format;
  if (color_type == PNG_COLOR_TYPE_RGB)
  {
    format = GL_RGB;
  }
  else
  {
    format = GL_RGBA;
  }

  png_read_update_info(png_ptr, info_ptr);

  int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  rowbytes += 3 - ((rowbytes - 1) % 4);
  png_byte* image_data = (png_byte*) malloc(rowbytes * height * sizeof(png_byte) + 15);
  if (image_data == NULL)
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(file);
    throw std::runtime_error("File loading failed, cannot allocate memory");
  }

  png_byte** row_pointers = (png_byte**) malloc(height * sizeof(png_byte*));
  if (row_pointers == NULL)
  {
      free(image_data);
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
      fclose(file);
      throw std::runtime_error("File loading failed, cannot allocate memory");
  }

  for (unsigned int i = 0; i < height; i++)
  {
      row_pointers[height - 1 - i] = image_data + i * rowbytes;
  }

  png_read_image(png_ptr, row_pointers);

  // Generate the OpenGL texture
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(
    GL_TEXTURE_2D, 0, format, width, height, 0,
    format, GL_UNSIGNED_BYTE, image_data);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // clean up memory
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  free(image_data);
  free(row_pointers);
  fclose(file);

  return texture;
}

template<typename Out>
void split(const std::string &s, char delim, Out result){
  std::stringstream ss;
  ss.str(s);
  std::string item;

  while(std::getline(ss, item, delim)){
    *(result++) = item;
  }
}

std::vector<std::string> split(const std::string &s, char delim){
  std::vector<std::string> elems;
  split(s, delim, std::back_inserter(elems));

  return elems;
}

bool str_equal(const std::string& str1, const std::string& str2){
  return str1.compare(str2) == 0;
}

Vector4f extractColor(const std::string& str){
  std::vector<std::string> strColors = split(str, ',');
  std::vector<float> colors;

  for(const auto& strColor: strColors){
    float value = 0;
    if(not str_equal(strColor, "0")){
      value = std::stof(strColor) / 255.;
    }
    colors.push_back(value);
  }

  return Vector4f(colors.at(0), colors.at(1), colors.at(2), 1.0);
}

Config loadConfig(const std::string& path){
  Config config;

  std::ifstream fobj(path);
  std::string line;

  while(std::getline(fobj, line)){
    std::vector<std::string> splittedLine = split(line, ' ');

    if(str_equal(splittedLine.at(0), "mode")){
      if(str_equal(splittedLine.at(1), "window")){
        config.mode = WINDOW;
      }
      else if(str_equal(splittedLine.at(1), "fullscreen")){
        config.mode = FULLSCREEN;
      }

      continue;
    }

    if(str_equal(splittedLine.at(0), "resolution")){
      std::vector<std::string> resolution = split(splittedLine.at(1), 'x');
      config.resolution = {
        std::stoi(resolution.at(0)), std::stoi(resolution.at(1))
      };

      continue;
    }

    if(str_equal(splittedLine.at(0), "shadows")){
      if(str_equal(splittedLine.at(1), "high")){
        config.shadows = SHADOWMAPPING_HIGH;
      }
      else if(str_equal(splittedLine.at(1), "low")){
        config.shadows = SHADOWMAPPING_LOW;
      }
      else if(str_equal(splittedLine.at(1), "very") and
              str_equal(splittedLine.at(2), "low")){
        config.shadows = SHADOWMAPPING_VERY_LOW;
      }

      continue;
    }

    if(str_equal(splittedLine.at(0), "antialiasing")){
      if(str_equal(splittedLine.at(1), "high")){
        config.antialiasing = ANTIALIASING_HIGH;
      }
      else if(str_equal(splittedLine.at(1), "low")){
        config.antialiasing = ANTIALIASING_LOW;
      }
      else if(str_equal(splittedLine.at(1), "none")){
        config.antialiasing = ANTIALIASING_NONE;
      }

      continue;
    }

    if(str_equal(splittedLine.at(0), "difficulty")){
      if(str_equal(splittedLine.at(1), "impossible")){
        config.difficulty = DIFFICULTY_IMPOSSIBLE;
      }
      else if(str_equal(splittedLine.at(1), "hard")){
        config.difficulty = DIFFICULTY_HARD;
      }
      else if(str_equal(splittedLine.at(1), "normal")){
        config.difficulty = DIFFICULTY_NORMAL;
      }
      else if(str_equal(splittedLine.at(1), "easy")){
        config.difficulty = DIFFICULTY_EASY;
      }

      continue;
    }

    if(str_equal(splittedLine.at(0), "ai")){
      config.ai = splittedLine.at(1);

      continue;
    }

    if(str_equal(splittedLine.at(0), "show_suggested_move")){
      if(str_equal(splittedLine.at(1), "true")){
        config.show_suggested_move = true;
      }
      else if(str_equal(splittedLine.at(1), "false")){
        config.show_suggested_move = false;
      }

      continue;
    }

    if(str_equal(splittedLine.at(0), "user_pieces_color")){
      config.user_pieces_color = extractColor(splittedLine.at(1));

      continue;
    }

    if(str_equal(splittedLine.at(0), "user_smoke_color")){
      config.user_smoke_color = extractColor(splittedLine.at(1));

      continue;
    }

    if(str_equal(splittedLine.at(0), "ai_pieces_color")){
      config.ai_pieces_color = extractColor(splittedLine.at(1));

      continue;
    }

    if(str_equal(splittedLine.at(0), "ai_smoke_color")){
      config.ai_smoke_color = extractColor(splittedLine.at(1));

      continue;
    }

    if(str_equal(splittedLine.at(0), "background_color")){
      config.background_color = extractColor(splittedLine.at(1));

      continue;
    }

    if(str_equal(splittedLine.at(0), "board_color_1")){
      config.board_color_1 = extractColor(splittedLine.at(1));

      continue;
    }

    if(str_equal(splittedLine.at(0), "board_color_2")){
      config.board_color_2 = extractColor(splittedLine.at(1));

      continue;
    }

    if(str_equal(splittedLine.at(0), "allowed_move_color")){
      config.allowed_move_color = extractColor(splittedLine.at(1));

      continue;
    }

    return config;
  }
};

bool _displayGLErrors(const char *file, int line){
  GLenum errorCode;
  bool foundError(false);

  while((errorCode = glGetError()) != GL_NO_ERROR){
    std::string error = "UNKNOWN_ERROR";

    switch(errorCode) {
      case GL_INVALID_OPERATION:
        error = "GL_INVALID_OPERATION";
        break;
      case GL_INVALID_ENUM:
        error = "GL_INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        error = "GL_INVALID_VALUE";
        break;
      case GL_OUT_OF_MEMORY:
        error = "GL_OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        error = "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
      case GL_STACK_UNDERFLOW:
        error = "GL_STACK_UNDERFLOW";
        break;
      case GL_STACK_OVERFLOW:
        error = "GL_STACK_OVERFLOW";
        break;
    }

    std::cerr << file << ":" << line << " -> Error code: "
      << errorCode << "(" << error.c_str() << ")"  << std::endl;
    foundError = true;
  }

  return foundError;
}
