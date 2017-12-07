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
#include "GameInfo.hxx"
#include "utils.hxx"

#include "chessBoard/chessBoard.hxx"

/* Perform a cel-shading rendering in the current frameBuffer
  \param gameInfo The current game informations
  \param meshes The map of meshes
  \param programs The map of shader programs
  \param shadowMap The shadowMap texture
*/
void celShadingRender(
  GameInfo* gameInfo,
  std::map<int, Mesh*>* meshes,
  std::map<int, ShaderProgram*>* programs,
  GLuint shadowMap);

void shadowMappingRender(
  GameInfo* gameInfo,
  std::map<int, Mesh*>* meshes,
  std::map<int, ShaderProgram*>* programss);

int main(){
  // Initialize game informations
  GameInfo gameInfo;

  // Create window
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = gameInfo.antialiasingLevel;
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
  window.setVerticalSyncEnabled(gameInfo.vSync);
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

  // Compute cameraLookAtMatrix and cameraProjectionMatrix
  sf::Vector3f center = {0.0, 0.0, 0.0};
  sf::Vector3f up = {0.0, 0.0, 1.0};
  gameInfo.cameraViewMatrix = getLookAtMatrix(
    gameInfo.cameraPosition, center, up
  );
  gameInfo.cameraProjectionMatrix = getPerspectiveProjMatrix(
    gameInfo.fovy, (double)width/height, 1, 1000
  );

  // Create orthographic projection matrix for shadow mapping
  gameInfo.lightProjectionMatrix = getOrthoProjMatrix(
    -20, 20, -20, 20, 1, 40
  );

  // Get lookAt matrix from light position for shadow mapping
  sf::Vector3f lightPosition = {
    (float)-20.0 * gameInfo.lightDirection.x,
    (float)-20.0 * gameInfo.lightDirection.y,
    (float)-20.0 * gameInfo.lightDirection.z
  };
  gameInfo.lightViewMatrix = getLookAtMatrix(
    lightPosition, center, up);

  // Create FBO for shadow mapping
  GLuint shadowMapFBO = 0;
  glGenFramebuffers(1, &shadowMapFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

  // Create the shadowMap texture
  GLuint shadowMap;
  glGenTextures(1, &shadowMap);
  glBindTexture(GL_TEXTURE_2D, shadowMap);

  // Set the texture
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGB,
    gameInfo.shadowMapResolution, gameInfo.shadowMapResolution,
    0, GL_RGB, GL_UNSIGNED_BYTE, 0
  );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadowMap, 0
  );

  // Create render buffer for depth test
  GLuint depthRenderBuffer;
  glGenRenderbuffers(1, &depthRenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
  glRenderbufferStorage(
    GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
    gameInfo.shadowMapResolution, gameInfo.shadowMapResolution
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

  // Render loop
  bool running = true;
  sf::Vector2i selectedPixelPosition = {0, 0};
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
        gameInfo.cameraProjectionMatrix = getPerspectiveProjMatrix(
          gameInfo.fovy, (double)width/height, 1, 1000
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
        if(event.key.code == sf::Keyboard::Down)
          gameInfo.cameraPosition.z -= 1.0;
        else if(event.key.code == sf::Keyboard::Up)
          gameInfo.cameraPosition.z += 1.0;
        else if(event.key.code == sf::Keyboard::Left)
          gameInfo.cameraPosition.x -= 1.0;
        else if(event.key.code == sf::Keyboard::Right)
          gameInfo.cameraPosition.x += 1.0;
        else continue;

        gameInfo.cameraViewMatrix = getLookAtMatrix(
          gameInfo.cameraPosition, center, up);
      }
    }

    // Create the shadowMap
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(
      0, 0, gameInfo.shadowMapResolution, gameInfo.shadowMapResolution);

    shadowMappingRender(&gameInfo, &meshes, &programs);

    // Do the cel-shading rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, width, height);

    // Display all pieces on the screen using the cel-shading effect
    celShadingRender(&gameInfo, &meshes, &programs, shadowMap);

    if(selecting){
      gameInfo.selectedPiecePosition = colorPicking->getClickedPiecePosition(
        selectedPixelPosition, &gameInfo, &meshes, &programs
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
    GameInfo* gameInfo,
    std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs,
    GLuint shadowMap){
  // The movement Matrix
  std::vector<GLfloat> movementMatrix;
  sf::Vector3f translation;
  sf::Vector3f rotation = {0, 0, 1};

  // Render all the black borders
  glUseProgram(programs->at(BLACK_BORDER)->id);
  glCullFace(GL_FRONT);

  // Bind uniform values
  programs->at(BLACK_BORDER)->setViewMatrix(&gameInfo->cameraViewMatrix[0]);
  programs->at(BLACK_BORDER)->setProjectionMatrix(
    &gameInfo->cameraProjectionMatrix[0]);

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = gameInfo->board[x][y];

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

      (gameInfo->selectedPiecePosition.x == x and
          gameInfo->selectedPiecePosition.y == y) ?
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
  programs->at(CEL_SHADING)->setViewMatrix(&gameInfo->cameraViewMatrix[0]);
  programs->at(CEL_SHADING)->setProjectionMatrix(
    &gameInfo->cameraProjectionMatrix[0]);
  programs->at(CEL_SHADING)->setUniformMatrix4fv(
    "LMatrix", &gameInfo->lightViewMatrix[0]);
  programs->at(CEL_SHADING)->setUniformMatrix4fv(
    "PLMatrix", &gameInfo->lightProjectionMatrix[0]);
  programs->at(CEL_SHADING)->bindTexture(
    0, GL_TEXTURE0, "shadowMap", shadowMap
  );

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = gameInfo->board[x][y];

      movementMatrix = getIdentityMatrix();
      // Rotate the piece depending on the team
      movementMatrix = piece > 0 ?
        rotate(&movementMatrix, -90.0, rotation) :
        rotate(&movementMatrix, 90.0, rotation);
      // Translate the piece
      translation = {(float)(x * 4.0 - 14.0), (float)(y * 4.0 - 14.0), 0.0};
      movementMatrix = translate(&movementMatrix, translation);
      programs->at(CEL_SHADING)->setMoveMatrix(&movementMatrix[0]);

      std::vector<GLfloat> normalMatrix = inverse(&movementMatrix);
      normalMatrix = transpose(&normalMatrix);
      programs->at(CEL_SHADING)->setNormalMatrix(&normalMatrix[0]);

      // Draw the board cell
      (x + y) % 2 == 0 ?
        programs->at(CEL_SHADING)->setUniform4f(
          "pieceColor", 0.70, 0.60, 0.41, 1.0) :
        programs->at(CEL_SHADING)->setUniform4f(
          "pieceColor", 1.0, 1.0, 1.0, 1.0);

      (gameInfo->selectedPiecePosition.x == x and
          gameInfo->selectedPiecePosition.y == y) ?
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
    GameInfo* gameInfo,
    std::map<int, Mesh*>* meshes,
    std::map<int, ShaderProgram*>* programs){
  // The movement Matrix
  std::vector<GLfloat> movementMatrix;
  sf::Vector3f translation;
  sf::Vector3f rotation = {0, 0, 1};

  // Render all meshes with the color depending on the depth (distance from
  // light)
  glUseProgram(programs->at(SHADOW_MAPPING)->id);
  glCullFace(GL_BACK);

  // Bind uniform values
  programs->at(SHADOW_MAPPING)->setViewMatrix(&gameInfo->lightViewMatrix[0]);
  programs->at(SHADOW_MAPPING)->setProjectionMatrix(
    &gameInfo->lightProjectionMatrix[0]);

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = gameInfo->board[x][y];

      movementMatrix = getIdentityMatrix();
      // Rotate the piece depending on the team
      movementMatrix = piece > 0 ?
        rotate(&movementMatrix, -90.0, rotation) :
        rotate(&movementMatrix, 90.0, rotation);
      // Translate the piece
      translation = {(float)(x * 4.0 - 14.0), (float)(y * 4.0 - 14.0), 0.0};
      movementMatrix = translate(&movementMatrix, translation);
      programs->at(SHADOW_MAPPING)->setMoveMatrix(&movementMatrix[0]);

      (gameInfo->selectedPiecePosition.x == x and
          gameInfo->selectedPiecePosition.y == y) ?
        programs->at(SHADOW_MAPPING)->setUniformBool("selected", true) :
        programs->at(SHADOW_MAPPING)->setUniformBool("selected", false);

      // Draw board cell
      meshes->at(BOARDCELL)->draw();

      if(piece != EMPTY) meshes->at(abs(piece))->draw();
    }
  }
};
