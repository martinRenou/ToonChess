#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "mesh/Mesh.hxx"


int main(){
  // Create and load king mesh
  Mesh* king = new Mesh("../assets/king.obj");
  king->load();

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

  // Create projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70, (double)width/height, 1, 1000);

  // Vertex buffers
  GLuint vertexBufferId;
  glGenBuffers(1, &vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glBufferData(
    GL_ARRAY_BUFFER,
    king->vertices.size()*sizeof(GLfloat),
    king->vertices.data(),
    GL_STATIC_DRAW);

  // Index buffer
  GLuint indexBufferId;
  glGenBuffers(1, &indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    king->indices.size()*sizeof(GLuint),
    king->indices.data(),
    GL_STATIC_DRAW);

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

    glEnableClientState(GL_VERTEX_ARRAY);

    // Draw
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glVertexPointer(
      3,
      GL_FLOAT,
      0,
      (void*)0
    );

    // Draw triangles
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glDrawElements(
      GL_TRIANGLES,
      king->indices.size(),
      GL_UNSIGNED_INT,
      (void*)0
    );

    glDisableClientState(GL_VERTEX_ARRAY);

    glFlush();

    window.display();
  }

  glDeleteBuffers(1, &vertexBufferId);
  glDeleteBuffers(1, &indexBufferId);

  return 0;
}
