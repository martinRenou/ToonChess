#define GL_GLEXT_PROTOTYPES

#include <GL/glu.h>

#include <SFML/Graphics.hpp>

#include <iostream>
#include <map>

#include "../mesh/Mesh.hxx"
#include "../shader/ShaderProgram.hxx"
#include "../constants.hxx"

#include "ColorPicking.hxx"

struct Pixel {
  GLfloat r;
  GLfloat g;
  GLfloat b;
};

/* Makes a color-picking rendering in the current framebuffer
  \param board The current board
  \param meshes The map of meshes
  \param programs The map of shader programs
*/
void colorPickingRender(
    int board[][8], std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs){
  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = board[x][y];

      glPushMatrix();

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(x * 4 - 14, y * 4 - 14, 0);

      glUseProgram(programs->at(COLOR_PICKING)->id);
      programs->at(COLOR_PICKING)->setUniform4f(
        "pieceColor", x/8.0, y/8.0, 0.0, 1.0);
      glCullFace(GL_BACK);

      // Display board cell
      meshes->at(BOARDCELL)->draw();

      if(piece != EMPTY && piece > 0){
        // Get mesh object
        Mesh* mesh = meshes->at(abs(piece));

        piece > 0 ?
          glRotatef(-90, 0, 0, 1) :
          glRotatef(90, 0, 0, 1);

        // Display colored piece (color depending on the position of the piece)
        mesh->draw();
      }

      GLint stackDepth;
      glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &stackDepth);

      if(stackDepth != 1) glPopMatrix();
    }
  }
};

ColorPicking::ColorPicking(GLuint width, GLuint height) :
  width{width}, height{height}{}

void ColorPicking::initBuffers(){
  // Create frameBuffer object
  glGenFramebuffers(1, &this->fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, this->fboId);

  // Create render buffer for color
  glGenRenderbuffers(1, &this->colorRenderBufferId);
  glBindRenderbuffer(GL_RENDERBUFFER, this->colorRenderBufferId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, this->width, this->height);
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    GL_RENDERBUFFER, this->colorRenderBufferId
  );

  // Create render buffer for depth test
  glGenRenderbuffers(1, &this->depthRenderBufferId);
  glBindRenderbuffer(GL_RENDERBUFFER, this->depthRenderBufferId);
  glRenderbufferStorage(
    GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->width, this->height);
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    GL_RENDERBUFFER, this->depthRenderBufferId
  );

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    std::cout << "Error while creating the FBO for color picking" << std::endl;
  }

  // Unbind buffers
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ColorPicking::resizeBuffers(GLuint width, GLuint height){
  // If the resize is a shrinking of the screen, then don't do anything because
  // the buffer will do the job
  if(this->width >= width and this->height >= height) return;

  // Change the size for the buffers
  this->width = width;
  this->height = height;

  this->deleteBuffers();

  // Recreate buffers with the new size
  this->initBuffers();
}

void ColorPicking::deleteBuffers(){
  // Delete framebuffer
  GLuint framebuffer[1] = {this->fboId};
  glDeleteFramebuffers(1, framebuffer);

  // Delete render buffers
  GLuint buffers[2] = {this->colorRenderBufferId, this->depthRenderBufferId};
  glDeleteRenderbuffers(2, buffers);
}

sf::Vector2i ColorPicking::getClickedPiecePosition(
    sf::Vector2i clickedPixelPosition,
    int board[][8], std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs){
  // Bind the framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, this->fboId);

  // Clear buffers and render
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  colorPickingRender(board, meshes, programs);

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
  this->deleteBuffers();
}
