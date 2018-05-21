#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

#include <iostream>
#include <map>
#include <cmath>

#include "../mesh/Mesh.hxx"
#include "../shader/ShaderProgram.hxx"
#include "../utils/math.hxx"
#include "../ChessGame/ChessGame.hxx"

#include "ShadowMapping.hxx"

void shadowMappingRender(
    ChessGame* game,
    std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs,
    DirectionalLight* light, float elapsedTime){
  // The movement Matrix
  std::vector<GLfloat> movementMatrix;
  Vector3f translation;
  Vector3f rotation = {0, 0, 1};

  // Get shader program
  ShaderProgram* shadowMappingProgram = programs->at(SHADOW_MAPPING);

  // Render all meshes with the color depending on the depth (distance from
  // light)
  glUseProgram(shadowMappingProgram->id);
  glCullFace(GL_BACK);

  // Bind uniform values
  shadowMappingProgram->setViewMatrix(&light->viewMatrix);
  shadowMappingProgram->setProjectionMatrix(&light->projectionMatrix);

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = game->board[x][y];

      // Set movement matrix to identity
      movementMatrix = getIdentityMatrix();

      // Rotate the piece depending on the team
      movementMatrix = piece > 0 ?
        rotate(&movementMatrix, -90.0, rotation) :
        rotate(&movementMatrix, 90.0, rotation);

      // Translate the piece
      translation = {(float)(x * 4.0 - 14.0), (float)(y * 4.0 - 14.0), 0.0};
      // If it's part of the suggested move
      if((game->suggestedUserMoveStartPosition.x == x and
          game->suggestedUserMoveStartPosition.y == y) or
          (game->suggestedUserMoveEndPosition.x == x and
          game->suggestedUserMoveEndPosition.y == y)){
        translation.z = 0.5 + 0.5 * sin(2*elapsedTime - M_PI/2.0);
      }
      movementMatrix = translate(&movementMatrix, translation);
      shadowMappingProgram->setMoveMatrix(&movementMatrix);

      // Draw board cell
      meshes->at(BOARDCELL)->draw();

      if(piece != EMPTY) meshes->at(abs(piece))->draw();
    }
  }

  // Display animated piece
  if(game->movingPiece != EMPTY){
    // Set movement matrix to identity
    movementMatrix = getIdentityMatrix();

    // Rotate the piece depending on the team (user or AI)
    movementMatrix = game->movingPiece > 0 ?
      rotate(&movementMatrix, -90.0, rotation) :
      rotate(&movementMatrix, 90.0, rotation);

    // Translate the piece
    translation = {
      (float)(game->movingPiecePosition.x * 4.0 - 14.0),
      (float)(game->movingPiecePosition.y * 4.0 - 14.0), 0.0
    };
    movementMatrix = translate(&movementMatrix, translation);
    shadowMappingProgram->setMoveMatrix(&movementMatrix);

    meshes->at(abs(game->movingPiece))->draw();
  }
};

ShadowMapping::ShadowMapping(GLuint resolution) : resolution{resolution}
{
}

void ShadowMapping::initBuffers(){
  // Create FBO for shadow mapping
  glGenFramebuffers(1, &fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);

  // Create the shadowMap texture
  glGenTextures(1, &shadowMapId);
  glBindTexture(GL_TEXTURE_2D, shadowMapId);

  // Set the texture
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGB,
    resolution, resolution,
    0, GL_RGB, GL_UNSIGNED_BYTE, 0
  );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadowMapId, 0
  );

  // Create render buffer for depth test
  glGenRenderbuffers(1, &depthRenderBufferId);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBufferId);
  glRenderbufferStorage(
    GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
    resolution, resolution
  );
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    GL_RENDERBUFFER, depthRenderBufferId
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
  GLuint framebuffer[1] = {fboId};
  glDeleteFramebuffers(1, framebuffer);

  // Delete render buffer
  GLuint buffers[1] = {depthRenderBufferId};
  glDeleteRenderbuffers(1, buffers);

  // Delete texture
  GLuint textures[1] = {shadowMapId};
  glDeleteTextures(1, textures);
}

void ShadowMapping::renderShadowMap(
    ChessGame* game, std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs,
    DirectionalLight* light, float elapsedTime){
  // Bind the framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);

  // Clear buffers and render
  glClearColor(1, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, resolution, resolution);

  shadowMappingRender(game, meshes, programs, light, elapsedTime);
};

GLuint ShadowMapping::getShadowMap(){
  return shadowMapId;
};


ShadowMapping::~ShadowMapping(){
  deleteBuffers();
};
