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
  std::ifstream fobj("../assets/king.obj");
  std::string line;
  std::vector<GLfloat> vertices;
  std::vector<GLfloat> normals;
  std::vector<GLuint> indices;

  while(std::getline(fobj, line))
  {
    std::vector<std::string> splittedLine = split(line, ' ');

    if(splittedLine.at(0).compare("v") == 0)
    {
      vertices.push_back(std::stof(splittedLine.at(1)));
      vertices.push_back(std::stof(splittedLine.at(2)));
      vertices.push_back(std::stof(splittedLine.at(3)));
      continue;
    }

    if(splittedLine.at(0).compare("vn") == 0)
    {
      normals.push_back(std::stof(splittedLine.at(1)));
      normals.push_back(std::stof(splittedLine.at(2)));
      normals.push_back(std::stof(splittedLine.at(3)));
      continue;
    }

    if(splittedLine.at(0).compare("f") == 0)
    {
      std::vector<std::string> splittedPoint = split(
        splittedLine.at(1),
        '/'
      );
      indices.push_back(std::stoi(splittedPoint.at(0)) - 1);

      std::vector<std::string> splittedPoint2 = split(
        splittedLine.at(2),
        '/'
      );
      indices.push_back(std::stoi(splittedPoint2.at(0)) - 1);

      std::vector<std::string> splittedPoint3 = split(
        splittedLine.at(3),
        '/'
      );
      indices.push_back(std::stoi(splittedPoint3.at(0)) - 1);
      continue;
    }
  }

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
    vertices.size()*sizeof(GLfloat),
    vertices.data(),
    GL_STATIC_DRAW);

  // Index buffer
  GLuint indexBufferId;
  glGenBuffers(1, &indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    indices.size()*sizeof(GLuint),
    indices.data(),
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
      indices.size(),
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
