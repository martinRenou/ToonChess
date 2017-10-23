#ifndef SHADER_HXX_
#define SHADER_HXX_

#include <GL/gl.h>

#include <string>

class Shader {
  public:
    /* The path of the file containing the GLSL code */
    std::string filePath;

    /* The type of the shader, must be one of GL_VERTEX_SHADER,
      GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER,
      GL_FRAGMENT_SHADER, or GL_COMPUTE_SHADER */
    GLenum type;

    /* The ID of the shader */
    GLuint id;

    /* Constructor
      \param filePath The path to the file containing the GLSL code
      \param type The type of the shader, must be one of GL_VERTEX_SHADER,
        GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER,
        GL_FRAGMENT_SHADER, or GL_COMPUTE_SHADER
    */
    Shader(std::string filePath, GLenum type);

    /* Compilation method, this will load the GLSL code and compile it
      \return True if the compilation went fine, false otherwise
    */
    bool compile();

    /* Destructor, this will remove the shader from memory */
    ~Shader();
};

#endif
