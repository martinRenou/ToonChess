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

#include "ColorPicking/ColorPicking.hxx"

#include "constants.hxx"
#include "utils.hxx"

#include "chessBoard/chessBoard.hxx"

/* Makes a cel-shading render in the current frameBuffer
  \param board The current board
  \param meshes The map of meshes
  \param programs The map of shader programs
  \param selectedPiecePosition The position of the selected piece on the board
  \param shadowMap The shadowMap texture
*/
void celShadingRender(
  int board[][8],
  std::map<int, Mesh*>* meshes,
  std::map<int, ShaderProgram*>* programs,
  sf::Vector2i* selectedPiecePosition,
  GLuint shadowMap);

void shadowMappingRender(
    int board[][8], std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs,
    sf::Vector2i* selectedPiecePosition);

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

  // Initialize color picking
  ColorPicking* colorPicking = new ColorPicking(width, height);
  colorPicking->initBuffers();

  // Get lookAt matrix
  std::vector<GLdouble> lookAtMatrix = getLookAtMatrix(
    0, -40, 20, 0, 0, 0, 0, 0, 1);

  // Create orthographic projection matrix for shadow mapping
  std::vector<GLdouble> orthoProjMatrix = getOrthoProjMatrix(
    -20, 20, -20, 20, 1, 40);

  // Get lookAt matrix from light position for shadow mapping
  std::vector<GLdouble> lightLookAtMatrix = getLookAtMatrix(
    20, 0, 20, 0, 0, 0, 0, 0, 1);

  // Create FBO for shadow mapping
  GLuint shadowMapFBO = 0;
  glGenFramebuffers(1, &shadowMapFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

  // Size of the shadowMap
  sf::Vector2i sizeShadowMap = {512, 512};

  // Create the shadowMap texture
  GLuint shadowMap;
  glGenTextures(1, &shadowMap);
  glBindTexture(GL_TEXTURE_2D, shadowMap);

  // Set the texture
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGB,
    sizeShadowMap.x, sizeShadowMap.y,
    0, GL_RGB, GL_UNSIGNED_BYTE, 0
  );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadowMap, 0);

  // Create render buffer for depth test
  GLuint depthRenderBuffer;
  glGenRenderbuffers(1, &depthRenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
  glRenderbufferStorage(
    GL_RENDERBUFFER, GL_DEPTH_COMPONENT, sizeShadowMap.x, sizeShadowMap.y
  );
  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer
  );

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    std::cout << "Error while creating the FBO for shadow mapping" << std::endl;
  }

  // Unbind
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
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

        // Resize the buffers for color picking
        colorPicking->resizeBuffers(width, height);
      }
      else if (event.type == sf::Event::MouseButtonReleased){
        if (event.mouseButton.button == sf::Mouse::Left){
          selectedPixelPosition.x = event.mouseButton.x;
          selectedPixelPosition.y = height - event.mouseButton.y;

          selecting = true;
        }
      }
    }

    // Create the shadowMap
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, sizeShadowMap.x, sizeShadowMap.y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixd(&orthoProjMatrix[0]);

    glMultMatrixd(&lightLookAtMatrix[0]);
    glTranslated(-20, 0, -20);

    shadowMappingRender(board, &meshes, &programs, &selectedPiecePosition);

    // Do the cel-shading rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50, (double)width/height, 1, 1000);

    glMultMatrixd(&lookAtMatrix[0]);
    glTranslated(0, 40, -20);

    // Display all pieces on the screen using the cel-shading effect
    celShadingRender(
      board, &meshes, &programs, &selectedPiecePosition, shadowMap
    );

    if(selecting){
      selectedPiecePosition = colorPicking->getClickedPiecePosition(
        selectedPixelPosition, board, &meshes, &programs
      );

      selecting = false;
    }

    glFlush();

    window.display();
  }

  deleteMeshes(&meshes);
  deletePrograms(&programs);
  delete colorPicking;

  return 0;
}

void celShadingRender(
    int board[][8], std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs,
    sf::Vector2i* selectedPiecePosition,
    GLuint shadowMap){
  glPushMatrix();

  // Render all the black borders
  glUseProgram(programs->at(BLACK_BORDER)->id);
  glCullFace(GL_FRONT);
  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = board[x][y];

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(x * 4 - 14, y * 4 - 14, 0);

      (selectedPiecePosition->x == x && selectedPiecePosition->y == y) ?
        programs->at(CEL_SHADING)->setUniformBool("selected", true) :
        programs->at(CEL_SHADING)->setUniformBool("selected", false);

      // Draw board cell
      meshes->at(BOARDCELL)->draw();

      if(piece != EMPTY){
        // Get piece mesh object
        Mesh* mesh = meshes->at(abs(piece));

        // Rotate it depending on the team
        piece > 0 ?
          glRotatef(-90, 0, 0, 1) :
          glRotatef(90, 0, 0, 1);

        mesh->draw();
      }
    }
  }

  // Render all meshes with cell shading
  glUseProgram(programs->at(CEL_SHADING)->id);
  glCullFace(GL_BACK);

  // Bind the shadow map
  programs->at(CEL_SHADING)->bindTexture(
    0, GL_TEXTURE0, "shadowMap", shadowMap
  );
  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = board[x][y];

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(x * 4 - 14, y * 4 - 14, 0);

      // Draw the board cell
      (x + y) % 2 == 0 ?
        programs->at(CEL_SHADING)->setUniform4f(
          "pieceColor", 0.70, 0.60, 0.41, 1.0) :
        programs->at(CEL_SHADING)->setUniform4f(
          "pieceColor", 1.0, 1.0, 1.0, 1.0);

      (selectedPiecePosition->x == x && selectedPiecePosition->y == y) ?
        programs->at(CEL_SHADING)->setUniformBool("selected", true) :
        programs->at(CEL_SHADING)->setUniformBool("selected", false);

      meshes->at(BOARDCELL)->draw();

      if(piece != EMPTY){
        // Get piece mesh object
        Mesh* mesh = meshes->at(abs(piece));

        // Rotate it depending on the team
        piece > 0 ?
          glRotatef(-90, 0, 0, 1) :
          glRotatef(90, 0, 0, 1);

        // Display cel-shading mesh
        piece > 0 ?
          programs->at(CEL_SHADING)->setUniform4f(
            "pieceColor", 1.0, 0.93, 0.70, 1.0) :
          programs->at(CEL_SHADING)->setUniform4f(
            "pieceColor", 0.51, 0.08, 0.08, 1.0);

        (selectedPiecePosition->x == x && selectedPiecePosition->y == y) ?
          programs->at(CEL_SHADING)->setUniformBool("selected", true) :
          programs->at(CEL_SHADING)->setUniformBool("selected", false);

        mesh->draw();
      }
    }
  }

  GLint stackDepth;
  glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &stackDepth);

  if(stackDepth != 1) glPopMatrix();
};

void shadowMappingRender(
    int board[][8], std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs,
    sf::Vector2i* selectedPiecePosition){
  glPushMatrix();

  // Render all meshes
  glUseProgram(programs->at(SHADOW_MAPPING)->id);
  glCullFace(GL_BACK);
  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = board[x][y];

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glTranslatef(x * 4 - 14, y * 4 - 14, 0);

      (selectedPiecePosition->x == x && selectedPiecePosition->y == y) ?
        programs->at(SHADOW_MAPPING)->setUniformBool("selected", true) :
        programs->at(SHADOW_MAPPING)->setUniformBool("selected", false);

      // Draw board cell
      meshes->at(BOARDCELL)->draw();

      if(piece != EMPTY){
        // Get piece mesh object
        Mesh* mesh = meshes->at(abs(piece));

        // Rotate it depending on the team
        piece > 0 ?
          glRotatef(-90, 0, 0, 1) :
          glRotatef(90, 0, 0, 1);

        mesh->draw();
      }
    }
  }

  GLint stackDepth;
  glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &stackDepth);

  if(stackDepth != 1) glPopMatrix();
};
