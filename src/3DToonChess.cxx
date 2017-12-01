#define GL_GLEXT_PROTOTYPES

#include <GL/glu.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <exception>
#include <map>
#include <iostream>
#include "math.h"

#include "mesh/Mesh.hxx"
#include "mesh/meshes.hxx"
#include "shader/ShaderProgram.hxx"
#include "shader/shaderPrograms.hxx"

#include "constants.hxx"
#include "utils.hxx"

#include "chessBoard/chessBoard.hxx"

/* Makes a cel-shading render in the current frameBuffer
  \param board The current board
  \param meshes The map of meshes
  \param programs The map of shader programs
  \param selectedPiecePosition The position of the selected piece on the board
*/
void celShadingRender(
  int board[][8],
  std::map<int, Mesh*>* meshes,
  std::map<int, ShaderProgram*>* programs,
  sf::Vector2i* selectedPiecePosition);

void colorPickingRender(
    int board[][8], std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs);

struct Pixel {
  GLfloat r;
  GLfloat g;
  GLfloat b;
};

int main(){
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
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Enable backface culling
  glEnable(GL_CULL_FACE);
  glClearColor(1, 1, 1, 1);

  // Create projection matrix
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50, (double)width/height, 1, 1000);

  // Load and compile shaders
  std::map<int, ShaderProgram*> programs;
  try{
    programs = initPrograms();
  } catch(const std::exception& e){
    std::cerr << e.what() << std::endl;
    return 1;
  }

  // Load meshes
  std::map<int, Mesh*> meshes = initMeshes();

  // Create frameBuffer object for color picking
  GLuint colorPickingFBO = 0;
  glGenFramebuffers(1, &colorPickingFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, colorPickingFBO);

  // Create render buffer for color
  GLuint colorRenderBuffer;
  glGenRenderbuffers(1, &colorRenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer
  );

  // Create render buffer for depth test
  GLuint depthRenderBuffer;
  glGenRenderbuffers(1, &depthRenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer
  );

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    std::cout << "Error while creating the FBO for color picking" << std::endl;

    deleteMeshes(&meshes);
    deletePrograms(&programs);

    return 1;
  }

  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //

  // Define board
  int board[8][8] = {
    ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK,
    KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT,
    BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP,
    KING, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KING,
    QUEEN, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*QUEEN,
    BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP,
    KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT,
    ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK,
  };

  // Render loop
  bool running = true;
  sf::Vector2i selectedPixelPosition = {0, 0};
  sf::Vector2i  selectedPiecePosition = {-1, -1};
  bool selecting = false;
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
        gluPerspective(50, (double)width/height, 1, 1000);
      }
      else if (event.type == sf::Event::MouseButtonReleased){
        if (event.mouseButton.button == sf::Mouse::Left){
          selectedPixelPosition.x = event.mouseButton.x;
          selectedPixelPosition.y = height - event.mouseButton.y;

          selecting = true;
        }
      }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gluLookAt(0, -40, 20, 0, 0, 0, 0, 0, 1);

    // Display all pieces on the screen using the cel-shading effect
    celShadingRender(board, &meshes, &programs, &selectedPiecePosition);

    if(selecting){
      // Do the color picking rendering
      glBindFramebuffer(GL_FRAMEBUFFER, colorPickingFBO);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      colorPickingRender(board, &meshes, &programs);

      // Get pixel color at clicked position
      Pixel pixel;
      glReadPixels(
        selectedPixelPosition.x, selectedPixelPosition.y,
        1, 1,
        GL_RGB, GL_FLOAT,
        &pixel
      );

      // Get piece position according to picked color
      int selectedX = (int)round(pixel.r*8);
      int selectedY = (int)round(pixel.g*8);
      selectedPiecePosition.x = (selectedX == 8) ? -1 : selectedX;
      selectedPiecePosition.y = (selectedY == 8) ? -1 : selectedY;

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      selecting = false;
    }

    glFlush();

    window.display();
  }

  deleteMeshes(&meshes);
  deletePrograms(&programs);

  return 0;
}

void celShadingRender(
    int board[][8], std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs,
    sf::Vector2i* selectedPiecePosition){
  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = board[x][y];

      glPushMatrix();

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(x * 4 - 14, y * 4 - 14, 0);

      // Display the board cell
      glUseProgram(programs->at(BLACK_BORDER)->id);
      glCullFace(GL_FRONT);
      meshes->at(BOARDCELL)->draw();

      glUseProgram(programs->at(CEL_SHADING)->id);
      (x + y) % 2 == 0 ?
        programs->at(CEL_SHADING)->setUniform4f(
          "pieceColor", 0.70, 0.60, 0.41, 1.0) :
        programs->at(CEL_SHADING)->setUniform4f(
          "pieceColor", 1.0, 1.0, 1.0, 1.0);
      glCullFace(GL_BACK);
      meshes->at(BOARDCELL)->draw();

      if(piece != EMPTY){
        // Get piece mesh object
        Mesh* mesh = meshes->at(abs(piece));

        // Rotate it depending on the team
        piece > 0 ?
          glRotatef(-90, 0, 0, 1) :
          glRotatef(90, 0, 0, 1);

        // Display black borders
        glUseProgram(programs->at(BLACK_BORDER)->id);
        glCullFace(GL_FRONT);
        mesh->draw();

        // Display cel-shading mesh
        glUseProgram(programs->at(CEL_SHADING)->id);
        // Use color for user or AI pieces
        piece > 0 ?
          programs->at(CEL_SHADING)->setUniform4f(
            "pieceColor", 1.0, 0.93, 0.70, 1.0) :
          programs->at(CEL_SHADING)->setUniform4f(
            "pieceColor", 0.51, 0.08, 0.08, 1.0);
        // Change color if it's the selected piece
        if(selectedPiecePosition->x == x && selectedPiecePosition->y == y){
          programs->at(CEL_SHADING)->setUniform4f(
            "pieceColor", 0.47, 0.70, 0.22, 1.0);
        }
        glCullFace(GL_BACK);
        mesh->draw();
      }

      GLint stackDepth;
      glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &stackDepth);

      if(stackDepth != 1) glPopMatrix();
    }
  }
};

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
