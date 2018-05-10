#include <sstream>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <png.h>

#include <GLFW/glfw3.h>

#include "utils.hxx"

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
