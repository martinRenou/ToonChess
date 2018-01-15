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
#include "utils/utils.hxx"
#include "utils/math.hxx"

#include "chessBoard/chessBoard.hxx"

#include "StockfishConnector/StockfishConnector.hxx"


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
  StockfishConnector* stockfishConnector = new StockfishConnector();
  // Start communication with stockfish
  try{
    stockfishConnector->startCommunication();
  } catch(const std::exception& e){
    std::cerr << e.what() << std::endl;

    delete stockfishConnector;

    return 1;
  }

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
      "ToonChess",
      sf::Style::Default,
      settings
  );
  window.setFramerateLimit(60);
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

    delete stockfishConnector;

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
  gameInfo.cameraPosition.x = 0.0;
  gameInfo.cameraPosition.y = - gameInfo.cameraRotationRadius;
  gameInfo.cameraPosition.z = 20.0;
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

  // Display OpenGL errors
  displayGLErrors();

  // State machine
  const int USER_TURN = 0;
  const int WAITING = 1;
  const int IA_TURN = 2;
  int gameState = USER_TURN;
  sf::Clock iaClock;
  sf::Time waitingElapsedTime;
  sf::Vector2i lastPosition, newPosition;

  // Render loop
  bool running = true;
  sf::Vector2i selectedPixelPosition = {0, 0};
  // Mouse movement
  sf::Vector2i mousePosition;
  // Rotation speed in radians per pixel
  GLfloat rotationSpeed = 0.002;
  GLint dX = 0;
  GLint dY = 0;
  // Rotation angle around Z axis
  GLfloat phi;
  // Rotation angle around X axis
  GLfloat teta;
  bool cameraMoving = false;
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
      else if(event.type == sf::Event::MouseButtonPressed){
        // If it's the right button, it's a camera movement
        if(event.mouseButton.button == sf::Mouse::Right){
          cameraMoving = true;

          mousePosition = sf::Mouse::getPosition(window);
        }
      }
      else if(event.type == sf::Event::MouseButtonReleased){
        // If it's the left button, it must be a piece selection
        if(event.mouseButton.button == sf::Mouse::Left){
          selectedPixelPosition.x = event.mouseButton.x;
          selectedPixelPosition.y = gameInfo.height - event.mouseButton.y;

          // Register last user clicked position
          lastPosition = gameInfo.selectedPiecePosition;

          // Get selected piece using color picking
          gameInfo.selectedPiecePosition = \
            colorPicking->getClickedPiecePosition(
              selectedPixelPosition, &gameInfo, &meshes, &programs
          );

          // Register new user clicked position
          newPosition = gameInfo.selectedPiecePosition;

          // If it's the user turn, check if he wants to move a piece
          if(gameState == USER_TURN
              and lastPosition.x != -1
              and newPosition.x != -1
              and gameInfo.board[lastPosition.x][lastPosition.y] > 0){

            movePiece(lastPosition, newPosition, gameInfo.board);

            // Unselect piece
            gameInfo.selectedPiecePosition = {-1, -1};

            // Transition to waiting state and restart the clock for measuring
            // waiting time
            gameState = WAITING;
            iaClock.restart();
          }
        }

        // If it's the right button, it's the end of a camera movement
        if(event.mouseButton.button == sf::Mouse::Right){
          cameraMoving = false;
        }
      }
      else if(event.type == sf::Event::MouseMoved and cameraMoving){
        dX = event.mouseMove.x - mousePosition.x;
        dY = event.mouseMove.y - mousePosition.y;

        mousePosition.x = event.mouseMove.x;
        mousePosition.y = event.mouseMove.y;

        phi -= rotationSpeed * (double)gameInfo.width/gameInfo.height * dX;
        teta += rotationSpeed * dY;

        // Constraint phi between -PI/2 and PI/2
        if(phi > M_PI / 2.0) phi = M_PI / 2.0;
        else if (phi < - M_PI / 2.0) phi = - M_PI / 2.0;

        // Constraint teta between 0.0 and PI/3
        if(teta > M_PI / 3.0) teta = M_PI / 3.0;
        else if (teta < 0.0) teta = 0.0;

        // Compute camera position according to the new rotation angle
        gameInfo.cameraPosition.x = 40 * sin(phi);
        gameInfo.cameraPosition.y = - 40 * cos(phi) * cos(teta);
        gameInfo.cameraPosition.z = 20 + 40 * sin(teta);

        gameInfo.cameraViewMatrix = getLookAtMatrix(
          gameInfo.cameraPosition, center, up);
      }
      else if(event.type == sf::Event::KeyPressed){
        if(event.key.code == sf::Keyboard::Escape){
          running = false;

          continue;
        }
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

    // If we waited one second or more, transition to IA_TURN state
    if(gameState == WAITING and iaClock.getElapsedTime().asSeconds() >= 1.0)
      gameState = IA_TURN;

    if(gameState == IA_TURN){
      // Get IA decision according to the last user move
      std::string lastUserMovement = getMovement(lastPosition, newPosition);
      std::string newMove = stockfishConnector->getNextIAMove(lastUserMovement);

      // If the IA tried to move one user's pawn, stop the game
      std::string iaMoveStartPositionUCI = newMove.substr(0, 2);
      sf::Vector2i iaMoveStartPosition = uciFormatToPosition(
        iaMoveStartPositionUCI);
      if(gameInfo.board[iaMoveStartPosition.x][iaMoveStartPosition.y] >= 0){
        std::cerr <<
          "\033[1;31mGame error:\033[0m A forbiden move has been performed!"
          << std::endl;

        deleteMeshes(&meshes);
        deletePrograms(&programs);
        delete colorPicking;
        delete shadowMapping;
        delete stockfishConnector;

        return 1;
      }

      movePiece(newMove, gameInfo.board);

      // Transition to USER_TURN state
      gameState = USER_TURN;
    }

    glFlush();

    window.display();
  }

  window.close();

  deleteMeshes(&meshes);
  deletePrograms(&programs);
  delete colorPicking;
  delete shadowMapping;
  delete stockfishConnector;

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

  // Get shader programs
  ShaderProgram* blackBorderProgram = programs->at(BLACK_BORDER);
  ShaderProgram* celShadingProgram = programs->at(CEL_SHADING);

  // Render all the black borders
  glUseProgram(blackBorderProgram->id);
  glCullFace(GL_FRONT);

  // Bind uniform values
  blackBorderProgram->setViewMatrix(&gameInfo->cameraViewMatrix);
  blackBorderProgram->setProjectionMatrix(&gameInfo->cameraProjectionMatrix);

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = gameInfo->board[x][y];

      // Set movement matrix to identity
      movementMatrix = getIdentityMatrix();

      // Rotate the piece depending on the team (user or IA)
      movementMatrix = piece > 0 ?
        rotate(&movementMatrix, -90.0, rotation) :
        rotate(&movementMatrix, 90.0, rotation);

      // Translate the piece
      translation = {(float)(x * 4.0 - 14.0), (float)(y * 4.0 - 14.0), 0.0};
      movementMatrix = translate(&movementMatrix, translation);
      blackBorderProgram->setMoveMatrix(&movementMatrix);

      // Set if the piece is the selected one or not
      (gameInfo->selectedPiecePosition.x == x and
          gameInfo->selectedPiecePosition.y == y) ?
        blackBorderProgram->setBoolean("selected", true) :
        blackBorderProgram->setBoolean("selected", false);

      // Draw board cell
      meshes->at(BOARDCELL)->draw();

      // Draw piece
      if(piece != EMPTY) meshes->at(abs(piece))->draw();
    }
  }

  // Render all meshes with cell shading
  glUseProgram(celShadingProgram->id);
  glCullFace(GL_BACK);

  // Bind uniform values
  celShadingProgram->setViewMatrix(&gameInfo->cameraViewMatrix);
  celShadingProgram->setProjectionMatrix(&gameInfo->cameraProjectionMatrix);

  celShadingProgram->setMatrix4fv("LMatrix", &gameInfo->lightViewMatrix);
  celShadingProgram->setMatrix4fv("PLMatrix", &gameInfo->lightProjectionMatrix);

  // Bind shadow map texture
  celShadingProgram->bindTexture(0, GL_TEXTURE0, "shadowMap", shadowMap);

  // Set shadow map resolution
  celShadingProgram->setInt(
    "shadowMapResolution", gameInfo->shadowMapResolution
  );

  // Set lightDirection
  celShadingProgram->setVector3f(
    "lightDirection", gameInfo->lightDirection.x,
    gameInfo->lightDirection.y, gameInfo->lightDirection.z
  );

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = gameInfo->board[x][y];

      // Set movement matrix to identity
      movementMatrix = getIdentityMatrix();

      // Rotate the piece depending on the team (user or IA)
      movementMatrix = piece > 0 ?
        rotate(&movementMatrix, -90.0, rotation) :
        rotate(&movementMatrix, 90.0, rotation);

      // Translate the piece
      translation = {(float)(x * 4.0 - 14.0), (float)(y * 4.0 - 14.0), 0.0};
      movementMatrix = translate(&movementMatrix, translation);
      celShadingProgram->setMoveMatrix(&movementMatrix);

      // Compute normal matrix (=inverse(transpose(movementMatrix)))
      std::vector<GLfloat> normalMatrix = inverse(&movementMatrix);
      normalMatrix = transpose(&normalMatrix);
      celShadingProgram->setNormalMatrix(&normalMatrix);

      // Draw the checkerboard
      (x + y) % 2 == 0 ?
        celShadingProgram->setVector4f("color", 0.70, 0.60, 0.41, 1.0) :
        celShadingProgram->setVector4f("color", 1.0, 1.0, 1.0, 1.0);

      // Set if the piece is the selected one or not
      (gameInfo->selectedPiecePosition.x == x and
          gameInfo->selectedPiecePosition.y == y) ?
        celShadingProgram->setBoolean("selected", true) :
        celShadingProgram->setBoolean("selected", false);

      meshes->at(BOARDCELL)->draw();

      if(piece != EMPTY){
        // Display cel-shading mesh
        piece > 0 ?
          celShadingProgram->setVector4f("color", 1.0, 0.93, 0.70, 1.0) :
          celShadingProgram->setVector4f("color", 0.51, 0.08, 0.08, 1.0);

        meshes->at(abs(piece))->draw();
      }
    }
  }
};
