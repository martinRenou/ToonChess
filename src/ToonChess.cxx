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

#include "PhysicsWorld/PhysicsWorld.hxx"

#include "constants.hxx"
#include "GameInfo.hxx"
#include "utils/utils.hxx"
#include "utils/math.hxx"

#include "ChessGame/ChessGame.hxx"

/* Perform a cel-shading rendering in the current frameBuffer
  \param game The game instance
  \param gameInfo The current game informations
  \param pieces The map of meshes
  \param programs The map of shader programs
  \param shadowMap The shadowMap texture
*/
void celShadingRender(
  ChessGame* game,
  PhysicsWorld* physicsWorld,
  GameInfo* gameInfo,
  std::map<int, Mesh*>* pieces,
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
      "ToonChess",
      sf::Style::Default,
      settings
  );
  window.setFramerateLimit(30);
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

  // Create an instance of the Game (This starts the communication with
  // Stockfish and could fail)
  ChessGame* game = new ChessGame();
  try{
    game->start();
  } catch(const std::exception& e){
    std::cerr << e.what() << std::endl;

    delete game;

    return 1;
  }

  // Load pieces
  std::map<int, Mesh*> pieces = initPieces();

  // Load fragmented pieces
  std::map<int, std::vector<Mesh*>> fragmentMeshes = initFragmentMeshes();

  // Create physicsWorld
  PhysicsWorld* physicsWorld = new PhysicsWorld(&fragmentMeshes, game);

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

          // Get selected piece using color picking
          game->setNewSelectedPiecePosition(
            colorPicking->getClickedPiecePosition(
                selectedPixelPosition, game, &gameInfo, &pieces, &programs
            )
          );
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

    // Simulate dynamics world
    physicsWorld->simulate(game);

    // Create the shadowMap
    shadowMap = shadowMapping->getShadowMap(
      game, &gameInfo, &pieces, &programs);

    // Do the cel-shading rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, gameInfo.width, gameInfo.height);

    // Display all pieces on the screen using the cel-shading effect
    celShadingRender(
      game, physicsWorld, &gameInfo, &pieces, &programs, shadowMap);

    // Perform the chess rules
    try{
      game->perform();
    } catch(const std::exception& e){
      std::cerr << e.what() << std::endl;

      window.close();

      deletePieces(&pieces);
      deleteFragmentMeshes(&fragmentMeshes);
      deletePrograms(&programs);
      delete colorPicking;
      delete shadowMapping;
      delete game;
      delete physicsWorld;

      return 1;
    }

    glFlush();

    window.display();
  }

  window.close();

  deletePieces(&pieces);
  deleteFragmentMeshes(&fragmentMeshes);
  deletePrograms(&programs);
  delete colorPicking;
  delete shadowMapping;
  delete game;
  delete physicsWorld;

  return 0;
}

void celShadingRender(
    ChessGame* game,
    PhysicsWorld* physicsWorld,
    GameInfo* gameInfo,
    std::map<int, Mesh*>* pieces,
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

  // Display fragments black borders
  for(unsigned int i = 0; i < physicsWorld->fragmentPool.size(); i++){
    Fragment* fragment = physicsWorld->fragmentPool.at(i).second;

    // Set movement matrix
    movementMatrix = fragment->getMoveMatrix();
    blackBorderProgram->setMoveMatrix(&movementMatrix);

    // Compute normal matrix (=inverse(transpose(movementMatrix)))
    std::vector<GLfloat> normalMatrix = inverse(&movementMatrix);
    normalMatrix = transpose(&normalMatrix);
    blackBorderProgram->setNormalMatrix(&normalMatrix);

    blackBorderProgram->setBoolean("elevated", false);

    // Draw fragment
    fragment->mesh->draw();
  }

  // Display pieces
  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = game->board[x][y];

      // Set movement matrix to identity
      movementMatrix = getIdentityMatrix();

      // Rotate the piece depending on the team (user or AI)
      movementMatrix = piece > 0 ?
        rotate(&movementMatrix, -90.0, rotation) :
        rotate(&movementMatrix, 90.0, rotation);

      // Translate the piece
      translation = {(float)(x * 4.0 - 14.0), (float)(y * 4.0 - 14.0), 0.0};
      movementMatrix = translate(&movementMatrix, translation);
      blackBorderProgram->setMoveMatrix(&movementMatrix);

      // If it's the selected piece, or if it's an allowed next move, move up
      // the piece
      (game->selectedPiecePosition.x == x and
          game->selectedPiecePosition.y == y) or
          game->allowedNextPositions[x][y] ?
        blackBorderProgram->setBoolean("elevated", true) :
        blackBorderProgram->setBoolean("elevated", false);

      // Draw board cell
      pieces->at(BOARDCELL)->draw();

      // Draw piece
      if(piece != EMPTY) pieces->at(abs(piece))->draw();
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
    blackBorderProgram->setMoveMatrix(&movementMatrix);

    pieces->at(abs(game->movingPiece))->draw();
  }

  // Render all pieces with cell shading
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

  // Display fragments
  for(unsigned int i = 0; i < physicsWorld->fragmentPool.size(); i++){
    Fragment* fragment = physicsWorld->fragmentPool.at(i).second;

    // Set movement matrix
    movementMatrix = fragment->getMoveMatrix();
    celShadingProgram->setMoveMatrix(&movementMatrix);

    // Compute normal matrix (=inverse(transpose(movementMatrix)))
    std::vector<GLfloat> normalMatrix = inverse(&movementMatrix);
    normalMatrix = transpose(&normalMatrix);
    celShadingProgram->setNormalMatrix(&normalMatrix);

    // Compute color depending of the team
    physicsWorld->fragmentPool.at(i).first > 0 ?
      celShadingProgram->setVector4f("color", 1.0, 0.93, 0.70, 1.0) :
      celShadingProgram->setVector4f("color", 0.51, 0.08, 0.08, 1.0);
    celShadingProgram->setBoolean("elevated", false);

    // Draw fragment
    fragment->mesh->draw();
  }

  // Display pieces
  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      int piece = game->board[x][y];

      // Set movement matrix to identity
      movementMatrix = getIdentityMatrix();

      // Rotate the piece depending on the team (user or AI)
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

      // If it's part of the suggested user move, change its color
      if((game->suggestedUserMoveStartPosition.x == x and
          game->suggestedUserMoveStartPosition.y == y) or (
          game->suggestedUserMoveEndPosition.x == x and
          game->suggestedUserMoveEndPosition.y == y)){
        celShadingProgram->setVector4f("color", 0.40, 0.45, 0.70, 1.0);
      }

      // If it's the selected piece, or if it's an allowed next move, move up
      // the piece
      (game->selectedPiecePosition.x == x and
          game->selectedPiecePosition.y == y) or
          game->allowedNextPositions[x][y] ?
        celShadingProgram->setBoolean("elevated", true) :
        celShadingProgram->setBoolean("elevated", false);

      pieces->at(BOARDCELL)->draw();

      if(piece != EMPTY){
        // Display cel-shading mesh
        piece > 0 ?
          celShadingProgram->setVector4f("color", 1.0, 0.93, 0.70, 1.0) :
          celShadingProgram->setVector4f("color", 0.51, 0.08, 0.08, 1.0);

        pieces->at(abs(piece))->draw();
      }
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
    celShadingProgram->setMoveMatrix(&movementMatrix);

    // Compute normal matrix (=inverse(transpose(movementMatrix)))
    std::vector<GLfloat> normalMatrix = inverse(&movementMatrix);
    normalMatrix = transpose(&normalMatrix);
    celShadingProgram->setNormalMatrix(&normalMatrix);

    game->movingPiece > 0 ?
      celShadingProgram->setVector4f("color", 1.0, 0.93, 0.70, 1.0) :
      celShadingProgram->setVector4f("color", 0.51, 0.08, 0.08, 1.0);

    celShadingProgram->setBoolean("elevated", false);

    pieces->at(abs(game->movingPiece))->draw();
  }
};
