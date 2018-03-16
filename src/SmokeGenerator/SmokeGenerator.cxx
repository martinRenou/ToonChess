#define GL_GLEXT_PROTOTYPES

#include <vector>
#include <algorithm>
#include <iostream>

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include "SmokeGenerator.hxx"

bool compareByLifetime(SmokeParticle p1, SmokeParticle p2){
  return (p1.lifetime > p2.lifetime);
}

SmokeGenerator::SmokeGenerator(){
  // Create smoke particles
  for(int p = 0; p < maxNbParticles; p++){
    SmokeParticle particle;
    particle.speed = {0.0, 0.0, 0.0};
    particle.position = {0.0, 0.0, 0.0};
    particle.size = 0.0;
    particle.lifetime = 0.0;

    smokeParticles.push_back(particle);
  }

  // Start clock
  innerClock = new sf::Clock();
};

void SmokeGenerator::initBuffers(){
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

void SmokeGenerator::generate(sf::Vector3f position, int numberParticles){
  if(nbParticles + numberParticles > maxNbParticles){
    std::cout << "Cannot create more particles..." << std::endl;

    return;
  }

  // Random generators
  std::uniform_real_distribution<float> getPosition(-1.0, 1.0);
  std::uniform_real_distribution<float> getSize(1.0, 1.5);
  std::uniform_real_distribution<float> getLifetime(2.0, 3.0);

  // Create particles
  for(int p = nbParticles; p < nbParticles + numberParticles; p++){
    SmokeParticle& particle = smokeParticles.at(p);
    particle.speed = {0.1, 0.1, 0.2};
    particle.position = {
      position.x + getPosition(generator),
      position.y + getPosition(generator),
      position.z + getPosition(generator)
    };
    particle.size = getSize(generator);
    particle.lifetime = getLifetime(generator);
  }

  // Sort particles by lifetime
  std::sort(smokeParticles.begin(), smokeParticles.end(), compareByLifetime);

  nbParticles += numberParticles;
};

void SmokeGenerator::draw(){
  float timeSinceLastCall = innerClock->getElapsedTime().asSeconds();

  // Update positionSizeBuffer
  for(int p = 0; p < nbParticles; p++){
    SmokeParticle& particle = smokeParticles.at(p);

    // Update particle lifetime
    particle.lifetime -= timeSinceLastCall;
    if(particle.lifetime <= 0.0){
      particle.lifetime = 0.0;
      nbParticles -= 1;

      continue;
    }

    // Update particle position
    particle.position.x += particle.speed.x * timeSinceLastCall;
    particle.position.y += particle.speed.y * timeSinceLastCall;
    particle.position.z += particle.speed.z * timeSinceLastCall;

    // Update particle size
    particle.size *= particle.lifetime;

    // Update the buffer
    positionSizeBuffer[4 * p + 0] = particle.position.x;
    positionSizeBuffer[4 * p + 1] = particle.position.y;
    positionSizeBuffer[4 * p + 2] = particle.position.z;
    positionSizeBuffer[4 * p + 3] = particle.size;
  }

  // Bind the new positionSizeBuffer
  glBindBuffer(GL_ARRAY_BUFFER, positionSizeBufferId);
  glBufferData(
    GL_ARRAY_BUFFER,
    maxNbParticles * 4 * sizeof(GLfloat),
    NULL,
    GL_STREAM_DRAW);
  glBufferSubData(
    GL_ARRAY_BUFFER,
    0,
    maxNbParticles * 4 * sizeof(GLfloat),
    positionSizeBuffer);

  // Send vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // Send positions and sizes
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, positionSizeBufferId);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // Always use 4 vertices
  glVertexAttribDivisor(0, 0);
  // Always use one position/size per quad
  glVertexAttribDivisor(1, 1);

  // Draw triangles
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, nbParticles);

  // Restart clock
  innerClock->restart();
};

SmokeGenerator::~SmokeGenerator(){
  glDeleteBuffers(1, &vertexBufferId);
  glDeleteBuffers(1, &positionSizeBufferId);
};
