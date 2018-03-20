#define GL_GLEXT_PROTOTYPES

#include <vector>
#include <algorithm>
#include <iostream>

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include "../shader/shaderPrograms.hxx"

#include "SmokeGenerator.hxx"

bool compareByLifetime(SmokeParticle* p1, SmokeParticle* p2){
  return (p1->lifetime > p2->lifetime);
}

SmokeGenerator::SmokeGenerator(){
  // Create smoke particles
  for(int p = 0; p < maxNbParticles; p++){
    SmokeParticle* particle = new SmokeParticle;
    particle->speed = {0.0, 0.0, 0.0};
    particle->position = {0.0, 0.0, 0.0};
    particle->size = 0.0;
    particle->lifetime = 0.0;
    particle->textureIndex = 0;

    smokeParticles.push_back(particle);
  }

  // Create the smoke shader program
  smokeShaderProgram = createProgram(
    "../shaders/smokeVS.glsl",
    "../shaders/smokeFS.glsl"
  );

  try{
    smokeShaderProgram->compile();
  } catch(const std::exception& e){
    // If something went wrong, delete the program and forward the exception
    delete smokeShaderProgram;

    throw;
  }

  // Load texture
  smokeTexture0 = new sf::Texture();
  if (!smokeTexture0->loadFromFile("../assets/smoke_texture0.png"))
    std::cout << "Couldn't load smoke texture file..." << std::endl;
  smokeTexture1 = new sf::Texture();
  if (!smokeTexture1->loadFromFile("../assets/smoke_texture1.png"))
    std::cout << "Couldn't load smoke texture file..." << std::endl;
  smokeTexture2 = new sf::Texture();
  if (!smokeTexture2->loadFromFile("../assets/smoke_texture2.png"))
    std::cout << "Couldn't load smoke texture file..." << std::endl;

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

  // TextureIndex buffer
  glGenBuffers(1, &textureIndexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, textureIndexBufferId);
  glBufferData(
    GL_ARRAY_BUFFER,
    maxNbParticles * 1 * sizeof(GLfloat),
    NULL,
    GL_STREAM_DRAW);
};

void SmokeGenerator::generate(sf::Vector3f position, int numberParticles){
  if(nbParticles + numberParticles > maxNbParticles){
    std::cout << "Cannot create more particles..." << std::endl;

    return;
  }

  // Random generators
  std::uniform_real_distribution<float> getPosition(-0.2, 1.5);
  std::uniform_real_distribution<float> getSize(1.0, 3.5);
  std::uniform_real_distribution<float> getLifetime(2.0, 3.0);
  std::uniform_real_distribution<float> getTextureIndex(0.0, 3.0);

  // Create particles
  for(int p = nbParticles; p < nbParticles + numberParticles; p++){
    SmokeParticle* particle = smokeParticles.at(p);
    particle->position = {
      position.x + getPosition(generator),
      position.y + getPosition(generator),
      position.z + getPosition(generator)
    };
    particle->size = getSize(generator);

    // Compute speed depending on the size
    float zSpeed = 3.0 * 1.0/particle->size;
    particle->speed = {0.1, 0.1, zSpeed};

    particle->lifetime = getLifetime(generator);
    particle->textureIndex = getTextureIndex(generator);
  }

  // Sort particles by lifetime
  std::sort(smokeParticles.begin(), smokeParticles.end(), compareByLifetime);

  nbParticles += numberParticles;
};

void SmokeGenerator::draw(GameInfo* gameInfo){
  float timeSinceLastCall = innerClock->getElapsedTime().asSeconds();

  if(nbParticles > 0){
    // Update positionSizeBuffer
    for(int p = 0; p < nbParticles; p++){
      SmokeParticle* particle = smokeParticles.at(p);

      // Update particle lifetime
      particle->lifetime -= timeSinceLastCall;
      if(particle->lifetime <= 0.0){
        particle->lifetime = 0.0;
        nbParticles -= 1;

        continue;
      }

      // Update particle position
      particle->position.x += particle->speed.x * timeSinceLastCall;
      particle->position.y += particle->speed.y * timeSinceLastCall;
      particle->position.z += particle->speed.z * timeSinceLastCall;

      // Shrink smoke when dying
      if(particle->lifetime <= 1.1)
        particle->size *= particle->lifetime;

      // Update the buffer
      positionSizeBuffer[4 * p + 0] = particle->position.x;
      positionSizeBuffer[4 * p + 1] = particle->position.y;
      positionSizeBuffer[4 * p + 2] = particle->position.z;
      positionSizeBuffer[4 * p + 3] = particle->size;

      textureIndexBuffer[p] = particle->textureIndex;
    }

    // Bind smoke shader program
    glUseProgram(smokeShaderProgram->id);

    // Disable face culling
    glDisable(GL_CULL_FACE);

    smokeShaderProgram->setViewMatrix(&gameInfo->cameraViewMatrix);
    smokeShaderProgram->setProjectionMatrix(&gameInfo->cameraProjectionMatrix);

    smokeShaderProgram->bindTexture(
      0, GL_TEXTURE0, "smokeTexture0", smokeTexture0);
    smokeShaderProgram->bindTexture(
      1, GL_TEXTURE1, "smokeTexture1", smokeTexture1);
    smokeShaderProgram->bindTexture(
      2, GL_TEXTURE2, "smokeTexture2", smokeTexture2);

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

    // Bind the new textureIndexBuffer
    glBindBuffer(GL_ARRAY_BUFFER, textureIndexBufferId);
    glBufferData(
      GL_ARRAY_BUFFER,
      maxNbParticles * 1 * sizeof(GLfloat),
      NULL,
      GL_STREAM_DRAW);
    glBufferSubData(
      GL_ARRAY_BUFFER,
      0,
      maxNbParticles * 1 * sizeof(GLfloat),
      textureIndexBuffer);

    // Send vertices
    glEnableVertexAttribArray(0);
    glBindAttribLocation(smokeShaderProgram->id, 0, "vertexPosition");
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Send positions and sizes
    glEnableVertexAttribArray(1);
    glBindAttribLocation(smokeShaderProgram->id, 1, "centerSize");
    glBindBuffer(GL_ARRAY_BUFFER, positionSizeBufferId);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Send texture indices
    glEnableVertexAttribArray(2);
    glBindAttribLocation(smokeShaderProgram->id, 2, "textureIndex");
    glBindBuffer(GL_ARRAY_BUFFER, textureIndexBufferId);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Always use 4 vertices
    glVertexAttribDivisor(0, 0);
    // Always use one position/size per quad
    glVertexAttribDivisor(1, 1);
    // Always use one textureIndex per quad
    glVertexAttribDivisor(2, 1);

    // Draw triangles
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, nbParticles);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    // Enable face culling
    glEnable(GL_CULL_FACE);
  }

  // Restart clock
  innerClock->restart();
};

SmokeGenerator::~SmokeGenerator(){
  glDeleteBuffers(1, &vertexBufferId);
  glDeleteBuffers(1, &positionSizeBufferId);

  for(unsigned int p = 0; p < smokeParticles.size(); p++)
    delete smokeParticles.at(p);

  delete smokeShaderProgram;
  delete smokeTexture0;
  delete smokeTexture1;
  delete smokeTexture2;
  delete innerClock;
};
