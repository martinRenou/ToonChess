#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include "SmokeGenerator.hxx"

SmokeGenerator::SmokeGenerator(){};

void SmokeGenerator::initBuffers(){
  static const GLfloat vertexBuffer[] = {
    -0.5f, 0.0f, -0.5f,
    0.5f, 0.0f, -0.5f,
    -0.5f, 0.0f, 0.5f,
    0.5f, 0.0f, 0.5f,
  };

  // Vertex buffer
  glGenBuffers(1, &vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(vertexBuffer),
    vertexBuffer,
    GL_STATIC_DRAW);

  // Position/Size buffer
  glGenBuffers(1, &positionSizeBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, positionSizeBufferId);
  glBufferData(
    GL_ARRAY_BUFFER,
    maxNbParticles * 4 * sizeof(GLfloat),
    NULL,
    GL_STREAM_DRAW);
};

void SmokeGenerator::generate(sf::Vector3f position){

};

void SmokeGenerator::draw(){
  // Send vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // Send positions and sizes
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, positionSizeBufferId);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // Draw triangles
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, nbParticles);
};

SmokeGenerator::~SmokeGenerator(){
  glDeleteBuffers(1, &vertexBufferId);
  glDeleteBuffers(1, &positionSizeBufferId);
};
