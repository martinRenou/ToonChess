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
#include "ShadowMapping/ShadowMapping.hxx"

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

  sf::Window window(
      sf::VideoMode(gameInfo.width, gameInfo.height),
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
  ColorPicking* colorPicking = new ColorPicking(
    gameInfo.width, gameInfo.height);
  colorPicking->initBuffers();

  // Initialize shadow mapping
  ShadowMapping* shadowMapping = new ShadowMapping(
    gameInfo.shadowMapResolution);
  shadowMapping->initBuffers();

  // Compute cameraLookAtMatrix and cameraProjectionMatrix
  sf::Vector3f center = {0.0, 0.0, 0.0};
  sf::Vector3f up = {0.0, 0.0, 1.0};
  gameInfo.cameraViewMatrix = getLookAtMatrix(
    gameInfo.cameraPosition, center, up
  );
  gameInfo.cameraProjectionMatrix = getPerspectiveProjMatrix(
    gameInfo.fovy, (double)gameInfo.width/gameInfo.height, 1, 1000
  );

  // Create orthographic projection matrix for shadow mapping
  gameInfo.lightProjectionMatrix = getOrthoProjMatrix(
    -25, 25, -20, 20, 1, 50
  );

  // Get lookAt matrix from light position for shadow mapping
  sf::Vector3f lightPosition = {
    (float)-20.0 * gameInfo.lightDirection.x,
    (float)-20.0 * gameInfo.lightDirection.y,
    (float)-20.0 * gameInfo.lightDirection.z
  };
  gameInfo.lightViewMatrix = getLookAtMatrix(
    lightPosition, center, up);

  // Render loop
  bool running = true;
  sf::Vector2i selectedPixelPosition = {0, 0};
  bool selecting = false;
  GLuint shadowMap;
  while(running){
    sf::Event event;
    while(window.pollEvent(event)){
      if(event.type == sf::Event::Closed){
        running = false;
      }
      else if(event.type == sf::Event::Resized){
        gameInfo.width = event.size.width;
        gameInfo.height = event.size.height;

        // Resize the buffers for color picking
        colorPicking->resizeBuffers(gameInfo.width, gameInfo.height);

        // Recompute projectionMatrix
        gameInfo.cameraProjectionMatrix = getPerspectiveProjMatrix(
          gameInfo.fovy, (double)gameInfo.width/gameInfo.height, 1, 1000
        );
      }
      else if(event.type == sf::Event::MouseButtonReleased){
        if(event.mouseButton.button == sf::Mouse::Left){
          selectedPixelPosition.x = event.mouseButton.x;
          selectedPixelPosition.y = gameInfo.height - event.mouseButton.y;

          selecting = true;
        }
      }
      else if(event.type == sf::Event::KeyPressed){
        if(event.key.code == sf::Keyboard::Escape){
          running = false;
          continue;
        }

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
    shadowMap = shadowMapping->getShadowMap(&gameInfo, &meshes, &programs);

    // Do the cel-shading rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, gameInfo.width, gameInfo.height);

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

  window.close();

  deleteMeshes(&meshes);
  deletePrograms(&programs);
  delete colorPicking;
  delete shadowMapping;

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

  programs->at(CEL_SHADING)->setUniform3f(
    "lightDirection", gameInfo->lightDirection.x,
    gameInfo->lightDirection.y, gameInfo->lightDirection.z
  );

  programs->at(CEL_SHADING)->setUniform1i(
    "shadowMapResolution", gameInfo->shadowMapResolution
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
