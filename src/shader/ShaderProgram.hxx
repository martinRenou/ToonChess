#ifndef SHADERPROGRAM_HXX_
#define SHADERPROGRAM_HXX_

#include <GL/gl.h>

#include <vector>

#include "Shader.hxx"

class ShaderProgram {
  public:
    /* The list of shaders to use in this program */
    std::vector<Shader*> shaders;

    /* The ID of the program */
    GLuint id;

    /* Constructor
      \param shaders The list of shaders to use in this program
    */
    explicit ShaderProgram(std::vector<Shader*> shaders);

    /* Compile method, this will compile and link the shaders together
      \throw CompilationException if a shader compilation is not a success
      \throw LinkingException if the linking of shaders is not a success
    */
    void compile();

    /* Destructor, this will remove the shaders from memory */
    ~ShaderProgram();
};

#endif
