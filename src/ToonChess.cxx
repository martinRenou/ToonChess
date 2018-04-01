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

#include "SmokeGenerator/SmokeGenerator.hxx"

#include "PhysicsWorld/PhysicsWorld.hxx"

#include "Event/EventStack.hxx"
#include "Event/Event.hxx"

#include "constants.hxx"

#include "Camera/Camera.hxx"
#include "DirectionalLight.hxx"

#include "utils/utils.hxx"
#include "utils/math.hxx"

#include "ChessGame/ChessGame.hxx"

/* Perform a cel-shading rendering in the current frameBuffer
  \param game The game instance
  \param physicsWorld The dynamics world
  \param pieces The map of meshes
  \param programs The map of shader programs
  \param shadowMap The shadowMaping instance
  \param camera The camera
*/
void celShadingRender(
  ChessGame* game,
  PhysicsWorld* physicsWorld,
  std::map<int, Mesh*>* pieces,
  std::map<int, ShaderProgram*>* programs,
  ShadowMapping* shadowMapping,
  Camera* camera,
  DirectionalLight* light);

int main(){
  // Create window
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = ANTIALIASING_LOW;
  settings.majorVersion = 3;
  settings.minorVersion = 0;

  GLint width = 1024;
  GLint height = 576;

  sf::Window window(
      sf::VideoMode(width, height),
      "ToonChess",
      sf::Style::Default,
      settings
  );
  window.setFramerateLimit(30);
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Enable backface culling
  glEnable(GL_CULL_FACE);

  // GLSL version
  std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

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
    deletePrograms(&programs);

    return 1;
  }

  // Create SmokeGenerator
  SmokeGenerator* smokeGenerator;
  try{
    smokeGenerator = new SmokeGenerator();
  } catch(const std::exception& e){
    std::cerr << e.what() << std::endl;

    delete game;
    deletePrograms(&programs);

    return 1;
  }
  smokeGenerator->initBuffers();

  // Load pieces
  std::map<int, Mesh*> pieces = initPieces();

  // Load fragmented pieces
  std::map<int, std::vector<Mesh*>> fragmentMeshes = initFragmentMeshes();

  // Create physicsWorld
  PhysicsWorld* physicsWorld = new PhysicsWorld(&fragmentMeshes, game);

  // Initialize color picking
  ColorPicking* colorPicking = new ColorPicking(width, height);
  colorPicking->initBuffers();

  // Initialize shadow mapping
  ShadowMapping* shadowMapping = new ShadowMapping();
  shadowMapping->initBuffers();

  // Create camera
  Camera* camera = new Camera((double)width/height);

  // Create orthographic projection matrix for shadow mapping
  DirectionalLight light;
  light.projectionMatrix = getOrthoProjMatrix(-25, 25, -20, 20, 1, 50);

  // Get lookAt matrix from light position for shadow mapping
  sf::Vector3f lightPosition = {
    (float)-20.0 * light.direction.x,
    (float)-20.0 * light.direction.y,
    (float)-20.0 * light.direction.z
  };
  light.viewMatrix = getLookAtMatrix(lightPosition, {0, 0, 0}, {0, 0, 1});

  // Display OpenGL errors
  displayGLErrors();

  // Render loop
  bool running = true;
  sf::Vector2i selectedPixelPosition = {0, 0};
  // Mouse movement
  sf::Vector2i mousePosition;
  GLint dX = 0;
  GLint dY = 0;
  bool cameraMoving = false;
  while(running){
    // Take care of SFML events
    sf::Event event;
    while(window.pollEvent(event)){
      if(event.type == sf::Event::Closed){
        running = false;
      }

      if(event.type == sf::Event::Resized){
        width = event.size.width;
        height = event.size.height;

        // Resize the buffers for color picking
        colorPicking->resizeBuffers(width, height);

        // Recompute camera perspective matrix
        camera->update((double)width/height);
      }

      if(event.type == sf::Event::MouseButtonPressed){
        // If it's the right button, it's a camera movement
        if(event.mouseButton.button == sf::Mouse::Right){
          cameraMoving = true;

          mousePosition = sf::Mouse::getPosition(window);
        }
      }

      if(event.type == sf::Event::MouseButtonReleased){
        // If it's the left button, it must be a piece selection
        if(event.mouseButton.button == sf::Mouse::Left){
          selectedPixelPosition.x = event.mouseButton.x;
          selectedPixelPosition.y = height - event.mouseButton.y;

          // Get selected piece using color picking
          game->setNewSelectedPiecePosition(
            colorPicking->getClickedPiecePosition(
                selectedPixelPosition, game, &pieces, &programs, camera
            )
          );
        }

        // If it's the right button, it's the end of a camera movement
        if(event.mouseButton.button == sf::Mouse::Right){
          cameraMoving = false;
        }
      }

      if(event.type == sf::Event::MouseMoved and cameraMoving){
        dX = event.mouseMove.x - mousePosition.x;
        dY = event.mouseMove.y - mousePosition.y;

        mousePosition.x = event.mouseMove.x;
        mousePosition.y = event.mouseMove.y;

        camera->move(dX, dY, (double)width/height);
      }

      if(event.type == sf::Event::KeyPressed){
        if(event.key.code == sf::Keyboard::Escape){
          running = false;

          continue;
        }
      }
    }

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
      delete smokeGenerator;
      delete game;
      delete physicsWorld;
      delete camera;

      return 1;
    }

    // Simulate dynamics world
    physicsWorld->simulate();

    // Take care of game events
    Event gameEvent;
    while(EventStack::pollEvent(&gameEvent)){
      if(gameEvent.type == Event::FragmentDisappearsEvent){
        // Generate smoke depending on the fragment volume and the team
        smokeGenerator->generate(
          gameEvent.fragment.position,
          (int)round(gameEvent.fragment.volume),
          gameEvent.fragment.piece > 0 ?
            sf::Vector3f(0.41, 0.37, 0.23) :
            sf::Vector3f(0.30, 0.12, 0.40),
          0.7
        );
      }

      if(gameEvent.type == Event::PieceTakenEvent){
        // Collapse piece in the dynamics world
        physicsWorld->collapsePiece(
          gameEvent.piece.piece,
          gameEvent.piece.position
        );
      }

      if(gameEvent.type == Event::PieceMovingEvent){
        // Update the piece position in the dynamics world
        physicsWorld->updatePiecePosition(
          gameEvent.movingPiece.startPosition,
          gameEvent.movingPiece.currentPosition
        );

        // Generate smoke
        smokeGenerator->generate(
          {
            gameEvent.movingPiece.currentPosition.x * 4 -14,
            gameEvent.movingPiece.currentPosition.y * 4 -14,
            0.0
          },
          1,
          sf::Vector3f(1.0, 1.0, 1.0),
          0.2
        );
      }

      if(gameEvent.type == Event::PieceStopsEvent){
        // Move the piece to its end position in the dynamics world
        physicsWorld->movePiece(
          gameEvent.movingPiece.startPosition,
          gameEvent.movingPiece.endPosition
        );
      }
    }

    // Perform rendering

    // Create the shadowMap
    shadowMapping->renderShadowMap(
      game, &pieces, &programs, &light);

    // Do the cel-shading rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, width, height);

    // Display all pieces on the screen using the cel-shading effect
    celShadingRender(
      game, physicsWorld, &pieces, &programs, shadowMapping, camera, &light);

    // Display smoke particles
    smokeGenerator->draw(camera);

    glFlush();

    window.display();
  }

  window.close();

  deletePieces(&pieces);
  deleteFragmentMeshes(&fragmentMeshes);
  deletePrograms(&programs);
  delete colorPicking;
  delete shadowMapping;
  delete smokeGenerator;
  delete game;
  delete physicsWorld;
  delete camera;

  return 0;
}

void celShadingRender(
    ChessGame* game,
    PhysicsWorld* physicsWorld,
    std::map<int, Mesh*>* pieces,
    std::map<int, ShaderProgram*>* programs,
    ShadowMapping* shadowMapping,
    Camera* camera,
    DirectionalLight* light){
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
  blackBorderProgram->setViewMatrix(&camera->viewMatrix);
  blackBorderProgram->setProjectionMatrix(&camera->projectionMatrix);

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
  celShadingProgram->setViewMatrix(&camera->viewMatrix);
  celShadingProgram->setProjectionMatrix(&camera->projectionMatrix);

  celShadingProgram->setMatrix4fv("LMatrix", &light->viewMatrix);
  celShadingProgram->setMatrix4fv("PLMatrix", &light->projectionMatrix);

  // Bind shadow map texture
  celShadingProgram->bindTexture(
    0, GL_TEXTURE0, "shadowMap", shadowMapping->getShadowMap());

  // Set shadow map resolution
  celShadingProgram->setInt(
    "shadowMapResolution", shadowMapping->resolution
  );

  // Set lightDirection
  celShadingProgram->setVector3f(
    "lightDirection",
    light->direction.x, light->direction.y, light->direction.z
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
