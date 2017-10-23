#include <GL/glu.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

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
  glEnable(GL_DEPTH_TEST);
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
    return 1;
  }

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

    gluLookAt(10, -10, 10, 0, 0, 0, 0, 0, 1);

    king->draw();

    glFlush();

    window.display();
  }

  delete toonVertexShader;

  return 0;
}
