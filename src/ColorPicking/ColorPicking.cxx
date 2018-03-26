#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>

#include <SFML/Graphics.hpp>

#include <iostream>
#include <map>

#include "../mesh/Mesh.hxx"
#include "../shader/ShaderProgram.hxx"
#include "../constants.hxx"
#include "../ChessGame/ChessGame.hxx"
#include "../utils/math.hxx"

#include "ColorPicking.hxx"

struct Pixel {
  GLfloat r;
  GLfloat g;
  GLfloat b;
};

/* Makes a color-picking rendering in the current framebuffer
  \param game The game instance
  \param meshes The map of meshes
  \param programs The map of shader programs
  \param camera The camera
*/
void colorPickingRender(
    ChessGame* game,
    std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs,
    Camera* camera){
  // The movement Matrix
  std::vector<GLfloat> movementMatrix;
  sf::Vector3f translation;
  sf::Vector3f rotation = {0, 0, 1};

  // Get shader program
  ShaderProgram* colorPickingProgram = programs->at(COLOR_PICKING);

  // Render everything with color depending on the position
  glUseProgram(colorPickingProgram->id);
  glCullFace(GL_BACK);

  // Bind uniform values
  colorPickingProgram->setViewMatrix(&camera->viewMatrix);
  colorPickingProgram->setProjectionMatrix(&camera->projectionMatrix);

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = game->board[x][y];

      // Set movement matrix
      movementMatrix = getIdentityMatrix();

      // Rotate the piece depending on the team
      movementMatrix = piece > 0 ?
        rotate(&movementMatrix, -90.0, rotation) :
        rotate(&movementMatrix, 90.0, rotation);

      // Translate the piece
      translation = {(float)(x * 4.0 - 14.0), (float)(y * 4.0 - 14.0), 0.0};
      movementMatrix = translate(&movementMatrix, translation);
      colorPickingProgram->setMoveMatrix(&movementMatrix);

      // If it's the selected piece, or if it's an allowed next move, move up
      // the piece
      (game->selectedPiecePosition.x == x and
          game->selectedPiecePosition.y == y) or
          game->allowedNextPositions[x][y] ?
        colorPickingProgram->setBoolean("elevated", true) :
        colorPickingProgram->setBoolean("elevated", false);

      // Set color depending on the position
      colorPickingProgram->setVector4f("color", x/8.0, y/8.0, 0.0, 1.0);

      // Display board cell
      meshes->at(BOARDCELL)->draw();

      if(piece != EMPTY) meshes->at(abs(piece))->draw();
    }
  }
};

ColorPicking::ColorPicking(GLuint width, GLuint height) :
  width{width}, height{height}{}

void ColorPicking::initBuffers(){
  // Create frameBuffer object
  glGenFramebuffers(1, &fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);

  // Create render buffer for color
  glGenRenderbuffers(1, &colorRenderBufferId);
  glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBufferId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    GL_RENDERBUFFER, colorRenderBufferId
  );

  // Create render buffer for depth test
  glGenRenderbuffers(1, &depthRenderBufferId);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBufferId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    GL_RENDERBUFFER, depthRenderBufferId
  );

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    std::cout << "Error while creating the FBO for color picking" << std::endl;
  }

  // Unbind buffers
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ColorPicking::resizeBuffers(GLuint newWidth, GLuint newHeight){
  // If the resize is a shrinking of the screen, then don't do anything because
  // the buffer will do the job
  if(width >= newWidth and height >= newHeight){
    width = newWidth;
    height = newHeight;

    return;
  };

  // Change the size for the buffers
  width = newWidth;
  height = newHeight;

  deleteBuffers();

  // Recreate buffers with the new size
  initBuffers();
}

void ColorPicking::deleteBuffers(){
  // Delete framebuffer
  GLuint framebuffer[1] = {fboId};
  glDeleteFramebuffers(1, framebuffer);

  // Delete render buffers
  GLuint buffers[2] = {colorRenderBufferId, depthRenderBufferId};
  glDeleteRenderbuffers(2, buffers);
}

sf::Vector2i ColorPicking::getClickedPiecePosition(
    sf::Vector2i clickedPixelPosition,
    ChessGame* game,
    std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs,
    Camera* camera){
  // Bind the framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);

  // Clear buffers and render
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, width, height);

  colorPickingRender(game, meshes, programs, camera);

  // Get pixel color at clicked position
  Pixel pixel;
  glReadPixels(
    clickedPixelPosition.x, clickedPixelPosition.y,
    1, 1,
    GL_RGB, GL_FLOAT,
    &pixel
  );

  // Get piece position according to picked color
  sf::Vector2i clickedPiecePosition;
  int selectedX = (int)round(pixel.r*8);
  int selectedY = (int)round(pixel.g*8);
  clickedPiecePosition.x = (selectedX == 8) ? -1 : selectedX;
  clickedPiecePosition.y = (selectedY == 8) ? -1 : selectedY;

  // Unbind framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return clickedPiecePosition;
}

ColorPicking::~ColorPicking(){
  deleteBuffers();
}
