#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <regex>

template<typename Out>
void split(const std::string &s, char delim, Out result) {
  std::stringstream ss;
  ss.str(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    *(result++) = item;
  }
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, std::back_inserter(elems));
  return elems;
}


int main()
{
  // Read obj file
  std::ifstream fobj("src/assets/king.obj");
  std::string line;
  std::vector<GLfloat> vertices;
  std::vector<GLfloat> normals;
  std::vector<GLuint> indices;
  while(std::getline(fobj, line))
  {
    std::vector<std::string> splittedLine = split(line, ' ');
    if(splittedLine.at(0).compare("v") == 0)
    {
      std::cout << "vertex ";
      std::cout << splittedLine.at(1) << splittedLine.at(2) << splittedLine.at(3) << std::endl;
      continue;
    }
    if(splittedLine.at(0).compare("vn") == 0)
    {
      std::cout << "normal ";
      std::cout << splittedLine.at(1) << splittedLine.at(2) << splittedLine.at(3) << std::endl;
      continue;
    }
    if(splittedLine.at(0).compare("f") == 0)
    {
      std::cout << "face ";
      std::cout << splittedLine.at(1) << splittedLine.at(2) << splittedLine.at(3) << std::endl;
      continue;
    }
  }
  std::cout << "end reading" << std::endl;

  // Create window
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = 4;
  settings.majorVersion = 3;
  settings.minorVersion = 0;

  sf::Window window(
      sf::VideoMode(800, 600),
      "3DToonChess",
      sf::Style::Default,
      settings
  );
  window.setVerticalSyncEnabled(true);
  glEnable(GL_DEPTH_TEST);

  // Create projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70, (double)800/600, 1, 1000);

  GLfloat VertexArray[] = {
    1.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f,
    1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f
  };

  GLuint IndexArray[] = {
    0,1,2,2,1,3,
    4,5,6,6,5,7,
    3,1,5,5,1,7,
    0,2,6,6,2,4,
    6,7,0,0,7,1,
    2,3,4,4,3,5
  };

  // Gen buffers
  GLuint vertexBufferId = 0;
  GLuint indexBufferId = 1;
  glGenBuffers(1, &vertexBufferId);
  glGenBuffers(1, &indexBufferId);

  // Vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(VertexArray),
    VertexArray,
    GL_STATIC_DRAW);

  // Index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    sizeof(IndexArray),
    IndexArray,
    GL_STATIC_DRAW);

  // Render loop
  bool running = true;
  while (running)
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
      {
        running = false;
      }
      else if (event.type == sf::Event::Resized)
      {
        glViewport(0, 0, event.size.width, event.size.height);
      }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(3, 3, 3, 0, 0, 0, 0, 0, 1);

    // Draw
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), ((float*)NULL + (3)));
    glColorPointer(3, GL_FLOAT, 6 * sizeof(float), 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Draw triangles
    glDrawElements(
      GL_TRIANGLES,
      sizeof(IndexArray)/sizeof(GLuint),
      GL_UNSIGNED_INT,
      0
    );

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glFlush();

    window.display();
  }

  glDeleteBuffers(1, &vertexBufferId);
  glDeleteBuffers(1, &indexBufferId);

  return 0;
}
