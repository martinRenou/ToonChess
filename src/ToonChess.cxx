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

#include "constants.hxx"

#include "Camera.hxx"
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

  // Compute cameraLookAtMatrix and cameraProjectionMatrix
  sf::Vector3f center = {0.0, 0.0, 0.0};
  sf::Vector3f up = {0.0, 0.0, 1.0};
  Camera camera;
  camera.position.x = 0.0;
  camera.position.y = - camera.radius;
  camera.position.z = 20.0;
  camera.viewMatrix = getLookAtMatrix(
    camera.position, center, up
  );
  camera.projectionMatrix = getPerspectiveProjMatrix(
    camera.fovy, (double)width/height, 1, 1000
  );

  // Create orthographic projection matrix for shadow mapping
  DirectionalLight light;
  light.projectionMatrix = getOrthoProjMatrix(-25, 25, -20, 20, 1, 50);

  // Get lookAt matrix from light position for shadow mapping
  sf::Vector3f lightPosition = {
    (float)-20.0 * light.direction.x,
    (float)-20.0 * light.direction.y,
    (float)-20.0 * light.direction.z
  };
  light.viewMatrix = getLookAtMatrix(lightPosition, center, up);

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
  GLfloat phi = 0.0;
  // Rotation angle around X axis
  GLfloat theta = 0.0;
  bool cameraMoving = false;
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
        camera.projectionMatrix = getPerspectiveProjMatrix(
          camera.fovy, (double)width/height, 1, 1000
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
          selectedPixelPosition.y = height - event.mouseButton.y;

          // Get selected piece using color picking
          game->setNewSelectedPiecePosition(
            colorPicking->getClickedPiecePosition(
                selectedPixelPosition, game, &pieces, &programs, &camera
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

        phi -= rotationSpeed * (double)width/height * dX;
        theta += rotationSpeed * dY;

        // Constraint phi between -PI/2 and PI/2
        if(phi > M_PI / 2.0) phi = M_PI / 2.0;
        else if (phi < - M_PI / 2.0) phi = - M_PI / 2.0;

        // Constraint theta between 0.0 and PI/3
        if(theta > M_PI / 3.0) theta = M_PI / 3.0;
        else if (theta < 0.0) theta = 0.0;

        // Compute camera position according to the new rotation angle
        camera.position.x = 40 * sin(phi);
        camera.position.y = - 40 * cos(phi) * cos(theta);
        camera.position.z = 20 + 40 * sin(theta);

        camera.viewMatrix = getLookAtMatrix(camera.position, center, up);
      }
      else if(event.type == sf::Event::KeyPressed){
        if(event.key.code == sf::Keyboard::Escape){
          running = false;

          continue;
        }
      }
    }

    // Simulate dynamics world
    physicsWorld->simulate(game, smokeGenerator);

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
      game, physicsWorld, &pieces, &programs, shadowMapping, &camera, &light);

    // Display smoke particles
    smokeGenerator->draw(&camera);

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
  delete smokeGenerator;
  delete game;
  delete physicsWorld;

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
