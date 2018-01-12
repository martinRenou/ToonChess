#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>

#include <iostream>
#include <map>

#include "../mesh/Mesh.hxx"
#include "../shader/ShaderProgram.hxx"
#include "../GameInfo.hxx"
#include "../utils/math.hxx"

#include "ShadowMapping.hxx"

void shadowMappingRender(
    GameInfo* gameInfo,
    std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs){
  // The movement Matrix
  std::vector<GLfloat> movementMatrix;
  sf::Vector3f translation;
  sf::Vector3f rotation = {0, 0, 1};

  // Get shader program
  ShaderProgram* shadowMappingProgram = programs->at(SHADOW_MAPPING);

  // Render all meshes with the color depending on the depth (distance from
  // light)
  glUseProgram(shadowMappingProgram->id);
  glCullFace(GL_BACK);

  // Bind uniform values
  shadowMappingProgram->setViewMatrix(&gameInfo->lightViewMatrix);
  shadowMappingProgram->setProjectionMatrix(&gameInfo->lightProjectionMatrix);

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = gameInfo->board[x][y];

      // Set movement matrix to identity
      movementMatrix = getIdentityMatrix();

      // Rotate the piece depending on the team
      movementMatrix = piece > 0 ?
        rotate(&movementMatrix, -90.0, rotation) :
        rotate(&movementMatrix, 90.0, rotation);

      // Translate the piece
      translation = {(float)(x * 4.0 - 14.0), (float)(y * 4.0 - 14.0), 0.0};
      movementMatrix = translate(&movementMatrix, translation);
      shadowMappingProgram->setMoveMatrix(&movementMatrix);

      // Set if the piece is selected or not
      (gameInfo->selectedPiecePosition.x == x and
          gameInfo->selectedPiecePosition.y == y) ?
        shadowMappingProgram->setBoolean("selected", true) :
        shadowMappingProgram->setBoolean("selected", false);

      // Draw board cell
      meshes->at(BOARDCELL)->draw();

      if(piece != EMPTY) meshes->at(abs(piece))->draw();
    }
  }
};

ShadowMapping::ShadowMapping(GLuint resolution) : resolution{resolution}{}

void ShadowMapping::initBuffers(){
  // Create FBO for shadow mapping
  glGenFramebuffers(1, &this->fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, this->fboId);

  // Create the shadowMap texture
  glGenTextures(1, &this->shadowMapId);
  glBindTexture(GL_TEXTURE_2D, this->shadowMapId);

  // Set the texture
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGB,
    this->resolution, this->resolution,
    0, GL_RGB, GL_UNSIGNED_BYTE, 0
  );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->shadowMapId, 0
  );

  // Create render buffer for depth test
  glGenRenderbuffers(1, &this->depthRenderBufferId);
  glBindRenderbuffer(GL_RENDERBUFFER, this->depthRenderBufferId);
  glRenderbufferStorage(
    GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
    this->resolution, this->resolution
  );
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    GL_RENDERBUFFER, this->depthRenderBufferId
  );

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    std::cout << "Error while creating the FBO for shadow mapping" << std::endl;
  }

  // Unbind
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

void ShadowMapping::deleteBuffers(){
  // Delete framebuffer
  GLuint framebuffer[1] = {this->fboId};
  glDeleteFramebuffers(1, framebuffer);

  // Delete render buffer
  GLuint buffers[1] = {this->depthRenderBufferId};
  glDeleteRenderbuffers(1, buffers);

  // Delete texture
  GLuint textures[1] = {this->shadowMapId};
  glDeleteTextures(1, textures);
}

GLuint ShadowMapping::getShadowMap(
    GameInfo* gameInfo, std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs){
  // Bind the framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, this->fboId);

  // Clear buffers and render
  glClearColor(1, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, this->resolution, this->resolution);

  shadowMappingRender(gameInfo, meshes, programs);

  // Return the shadowmap id
  return this->shadowMapId;
};

ShadowMapping::~ShadowMapping(){
  this->deleteBuffers();
};
