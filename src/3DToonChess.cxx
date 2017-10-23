#define GL_GLEXT_PROTOTYPES

#include <GL/glu.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>

#include "mesh/Mesh.hxx"
#include "shader/Shader.hxx"


int main(){
  // Create window
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = 4;
  settings.majorVersion = 3;
  settings.minorVersion = 0;

  int width(800);
  int height(600);
  sf::Window window(
      sf::VideoMode(width, height),
      "3DToonChess",
      sf::Style::Default,
      settings
  );
  window.setVerticalSyncEnabled(true);
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Enable backface culling
  glEnable(GL_CULL_FACE);
  glClearColor(0.69, 0.73, 1, 1);

  // Create projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70, (double)width/height, 1, 1000);

  // Load shaders
  Shader* toonVertexShader = new Shader(
    "../shaders/toonVertexShader.glsl",
    GL_VERTEX_SHADER
  );
  bool compilationIsSuccess = toonVertexShader->compile();
  if(!compilationIsSuccess){
    delete toonVertexShader;
    return 1;
  }

  Shader* toonFragmentShader = new Shader(
    "../shaders/toonFragmentShader.glsl",
    GL_FRAGMENT_SHADER
  );
  compilationIsSuccess = toonFragmentShader->compile();
  if(!compilationIsSuccess){
    delete toonVertexShader;
    delete toonFragmentShader;
    return 1;
  }

  // Create and link Shader program
  GLuint toonShaderProgram = glCreateProgram();
  glAttachShader(toonShaderProgram, toonVertexShader->id);
  glAttachShader(toonShaderProgram, toonFragmentShader->id);

  glLinkProgram(toonShaderProgram);

  //Note the different functions here: glGetProgram* instead of glGetShader*.
  GLint isLinked = 0;
  glGetProgramiv(toonShaderProgram, GL_LINK_STATUS, (int *)&isLinked);
  if(isLinked == GL_FALSE){
    GLint maxLength = 0;
    glGetProgramiv(toonShaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

    // Display linking error
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(toonShaderProgram, maxLength, &maxLength, &infoLog[0]);

    std::cout << "Unable to link program: " << std::endl
      << &infoLog[0] << std::endl;

    glDeleteProgram(toonShaderProgram);

    // Don't leak shaders either.
    delete toonVertexShader;
    delete toonFragmentShader;

    return 1;
  }

  glDetachShader(toonShaderProgram, toonVertexShader->id);
  glDetachShader(toonShaderProgram, toonFragmentShader->id);

  glUseProgram(toonShaderProgram);

  // Create and load king mesh
  Mesh* king = new Mesh("../assets/king.obj");
  king->initBuffers();

  // Render loop
  bool running = true;
  while(running){
    sf::Event event;
    while(window.pollEvent(event)){
      if(event.type == sf::Event::Closed){
        running = false;
      }
      else if(event.type == sf::Event::Resized){
        width = event.size.width;
        height = event.size.height;

        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(70, (double)width/height, 1, 1000);
      }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(10, -10, 10, 0, 0, 0, 0, 1, 0);

    king->draw();

    glFlush();

    window.display();
  }

  delete king;
  delete toonVertexShader;

  return 0;
}
