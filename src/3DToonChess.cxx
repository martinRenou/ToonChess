#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>

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
  GLuint shadowMap,
  GLfloat* lookAtMatrix,
  GLfloat* projectionMatrix);

void shadowMappingRender(
    int board[][8], std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs,
    sf::Vector2i* selectedPiecePosition,
    GLfloat* lookAtMatrix,
    GLfloat* projectionMatrix);

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

  // Compute lookAtMatrix and projectionMatrix
  sf::Vector3f eye = {0.0, -40.0, 20.0};
  sf::Vector3f center = {0.0, 0.0, 0.0};
  sf::Vector3f up = {0.0, 0.0, 1.0};
  std::vector<GLfloat> lookAtMatrix = getLookAtMatrix(eye, center, up);
  std::vector<GLfloat> projectionMatrix = getPerspectiveProjMatrix(
    50, (double)width/height, 1, 1000
  );

  // Create orthographic projection matrix for shadow mapping
  std::vector<GLfloat> orthoProjMatrix = getOrthoProjMatrix(
    -20, 20, -20, 20, 1, 40);

  // Get lookAt matrix from light position for shadow mapping
  sf::Vector3f lightPosition = {20, 0, 20};
  std::vector<GLfloat> lightLookAtMatrix = getLookAtMatrix(
    lightPosition, center, up);

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

        // Recompute projectionMatrix
        projectionMatrix = getPerspectiveProjMatrix(
          50, (double)width/height, 1, 1000
        );
      }
      else if(event.type == sf::Event::MouseButtonReleased){
        if(event.mouseButton.button == sf::Mouse::Left){
          selectedPixelPosition.x = event.mouseButton.x;
          selectedPixelPosition.y = height - event.mouseButton.y;

          selecting = true;
        }
      }
      else if(event.type == sf::Event::KeyPressed){
        if(event.key.code == sf::Keyboard::Down) eye.z -= 1.0;
        else if(event.key.code == sf::Keyboard::Up) eye.z += 1.0;
        else if(event.key.code == sf::Keyboard::Left) eye.x -= 1.0;
        else if(event.key.code == sf::Keyboard::Right) eye.x += 1.0;
        else continue;

        lookAtMatrix = getLookAtMatrix(eye, center, up);
      }
    }

    // Create the shadowMap
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, sizeShadowMap.x, sizeShadowMap.y);

    shadowMappingRender(
      board, &meshes, &programs, &selectedPiecePosition,
      &lightLookAtMatrix[0], &orthoProjMatrix[0]
    );

    // Do the cel-shading rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, width, height);

    // Display all pieces on the screen using the cel-shading effect
    celShadingRender(
      board, &meshes, &programs, &selectedPiecePosition,
      shadowMap, &lookAtMatrix[0], &projectionMatrix[0]
    );

    if(selecting){
      selectedPiecePosition = colorPicking->getClickedPiecePosition(
        selectedPixelPosition, board, &meshes, &programs,
        &lookAtMatrix[0], &projectionMatrix[0]
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
    GLuint shadowMap,
    GLfloat* lookAtMatrix,
    GLfloat* projectionMatrix){
  // The movement Matrix
  std::vector<GLfloat> movementMatrix;
  sf::Vector3f translation;
  sf::Vector3f rotation = {0, 0, 1};

  // Render all the black borders
  glUseProgram(programs->at(BLACK_BORDER)->id);
  glCullFace(GL_FRONT);

  // Bind uniform values
  programs->at(BLACK_BORDER)->setViewMatrix(lookAtMatrix);
  programs->at(BLACK_BORDER)->setProjectionMatrix(projectionMatrix);

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = board[x][y];

      // Set movement matrix
      movementMatrix = getIdentityMatrix();
      // Rotate the piece depending on the team
      movementMatrix = piece > 0 ?
        rotate(&movementMatrix, -90.0, rotation) :
        rotate(&movementMatrix, 90.0, rotation);
      // Translate the piece
      translation = {(float)(x * 4.0 - 14.0), (float)(y * 4.0 - 14.0), 0.0};
      movementMatrix = translate(&movementMatrix, translation);
      programs->at(BLACK_BORDER)->setMoveMatrix(&movementMatrix[0]);

      (selectedPiecePosition->x == x && selectedPiecePosition->y == y) ?
        programs->at(BLACK_BORDER)->setUniformBool("selected", true) :
        programs->at(BLACK_BORDER)->setUniformBool("selected", false);

      // Draw board cell
      meshes->at(BOARDCELL)->draw();

      if(piece != EMPTY) meshes->at(abs(piece))->draw();
    }
  }

  // Render all meshes with cell shading
  glUseProgram(programs->at(CEL_SHADING)->id);
  glCullFace(GL_BACK);

  // Bind uniform values
  programs->at(CEL_SHADING)->setViewMatrix(&lookAtMatrix[0]);
  programs->at(CEL_SHADING)->setProjectionMatrix(projectionMatrix);
  programs->at(CEL_SHADING)->bindTexture(
    0, GL_TEXTURE0, "shadowMap", shadowMap
  );

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = board[x][y];

      movementMatrix = getIdentityMatrix();
      // Rotate the piece depending on the team
      movementMatrix = piece > 0 ?
        rotate(&movementMatrix, -90.0, rotation) :
        rotate(&movementMatrix, 90.0, rotation);
      // Translate the piece
      translation = {(float)(x * 4.0 - 14.0), (float)(y * 4.0 - 14.0), 0.0};
      movementMatrix = translate(&movementMatrix, translation);
      programs->at(CEL_SHADING)->setMoveMatrix(&movementMatrix[0]);

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
        // Display cel-shading mesh
        piece > 0 ?
          programs->at(CEL_SHADING)->setUniform4f(
            "pieceColor", 1.0, 0.93, 0.70, 1.0) :
          programs->at(CEL_SHADING)->setUniform4f(
            "pieceColor", 0.51, 0.08, 0.08, 1.0);

        meshes->at(abs(piece))->draw();
      }
    }
  }
};

void shadowMappingRender(
    int board[][8], std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs,
    sf::Vector2i* selectedPiecePosition,
    GLfloat* lookAtMatrix,
    GLfloat* projectionMatrix){
  // The movement Matrix
  std::vector<GLfloat> movementMatrix;
  sf::Vector3f translation;
  sf::Vector3f rotation = {0, 0, 1};

  // Render all meshes with the color depending on the depth (distance from
  // light)
  glUseProgram(programs->at(SHADOW_MAPPING)->id);
  glCullFace(GL_BACK);

  // Bind uniform values
  programs->at(SHADOW_MAPPING)->setViewMatrix(lookAtMatrix);
  programs->at(SHADOW_MAPPING)->setProjectionMatrix(projectionMatrix);

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = board[x][y];

      movementMatrix = getIdentityMatrix();
      // Rotate the piece depending on the team
      movementMatrix = piece > 0 ?
        rotate(&movementMatrix, -90.0, rotation) :
        rotate(&movementMatrix, 90.0, rotation);
      // Translate the piece
      translation = {(float)(x * 4.0 - 14.0), (float)(y * 4.0 - 14.0), 0.0};
      movementMatrix = translate(&movementMatrix, translation);
      programs->at(SHADOW_MAPPING)->setMoveMatrix(&movementMatrix[0]);

      (selectedPiecePosition->x == x && selectedPiecePosition->y == y) ?
        programs->at(SHADOW_MAPPING)->setUniformBool("selected", true) :
        programs->at(SHADOW_MAPPING)->setUniformBool("selected", false);

      // Draw board cell
      meshes->at(BOARDCELL)->draw();

      if(piece != EMPTY) meshes->at(abs(piece))->draw();
    }
  }
};
