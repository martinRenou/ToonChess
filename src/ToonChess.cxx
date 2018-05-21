#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

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

#include "Clock/Clock.hxx"
#include "Camera/Camera.hxx"
#include "DirectionalLight.hxx"

#include "utils/utils.hxx"
#include "utils/math.hxx"

#include "get_config_path.hxx"

#include "ChessGame/ChessGame.hxx"

// Globals
bool resizing = false;
int width = 1024;
int height = 576;
bool cameraMoving = false;
int dX = 0;
int dY = 0;
Vector2i mousePosition;
bool selecting;

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
  DirectionalLight* light,
  Config* config,
  float elapsedTime);

void resize_callback(GLFWwindow* window, int new_width, int new_height)
{
  width = new_width;
  height = new_height;
  resizing = true;
};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
  {
    dX = 0;
    dY = 0;

    cameraMoving = true;
  }

  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
  {
    cameraMoving = false;
  }

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
  {
    selecting = true;
  }
};

void cursor_move_callback(GLFWwindow* window, double xpos, double ypos)
{
  int xposi = (int)xpos;
  int yposi = (int)ypos;

  dX = xposi - mousePosition.x;
  dY = yposi - mousePosition.y;

  mousePosition.x = xposi;
  mousePosition.y = yposi;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
    {
      glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main()
{
  // Load config
  Config config = loadConfig(get_config_path() + "config.txt");

  // Initialize glfw
  if (!glfwInit())
    return 1;

  // Antialiasing level
  if(config.antialiasing != ANTIALIASING_NONE)
  {
    glfwWindowHint(GLFW_SAMPLES, config.antialiasing);
  }

  // Create a window and its OpenGL context
  GLFWwindow* window;
  if(config.mode == FULLSCREEN)
  {
    window = glfwCreateWindow(width, height, "ToonChess", glfwGetPrimaryMonitor(), NULL);
  }
  else
  {
    window = glfwCreateWindow(width, height, "ToonChess", NULL, NULL);
  }
  if (!window)
  {
    glfwTerminate();
    return 1;
  }

  // Make the window's context current
  glfwMakeContextCurrent(window);

  if(config.antialiasing != ANTIALIASING_NONE)
  {
    glEnable(GL_MULTISAMPLE);
  }

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

    glfwTerminate();

    return 1;
  }

  // Create an instance of the Game (This starts the communication with
  // the AI and could fail)
  ChessGame* game = new ChessGame(config.ai, config.difficulty, config.show_suggested_move);
  try{
    game->start();
  } catch(const std::exception& e){
    std::cerr << e.what() << std::endl;

    delete game;
    deletePrograms(&programs);
    glfwTerminate();

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
    glfwTerminate();

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
  ShadowMapping* shadowMapping = new ShadowMapping(config.shadows);
  shadowMapping->initBuffers();

  // Main clock
  Clock mainClock;

  // Create camera
  Camera* camera = new Camera((double)width/height);

  // Create orthographic projection matrix for shadow mapping
  DirectionalLight light;
  light.projectionMatrix = getOrthoProjMatrix(-25, 25, -20, 20, 1, 50);

  // Get lookAt matrix from light position for shadow mapping
  Vector3f lightPosition = {
    (float)-20.0 * light.direction.x,
    (float)-20.0 * light.direction.y,
    (float)-20.0 * light.direction.z
  };
  light.viewMatrix = getLookAtMatrix(lightPosition, {0, 0, 0}, {0, 0, 1});

  // Display OpenGL errors
  displayGLErrors();

  // Set events callbacks
  glfwSetFramebufferSizeCallback(window, resize_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, cursor_move_callback);
  glfwSetKeyCallback(window, key_callback);

  // Render loop
  while(!glfwWindowShouldClose(window))
  {
    // Get elapsed time since game started
    float elapsedTime = mainClock.getElapsedTime();

    // Take care of glfw events
    glfwPollEvents();
    if (resizing)
    {
      // Resize the buffers for color picking
      colorPicking->resizeBuffers(width, height);

      // Recompute camera perspective matrix
      camera->updatePerspective((double)width/height);

      resizing = false;
    }
    if (cameraMoving)
    {
      camera->move(dX, dY, (double)width/height);
    }
    if (selecting)
    {
      // Get selected piece using color picking
      game->setNewSelectedPiecePosition(
        colorPicking->getClickedPiecePosition(
            {mousePosition.x, height - mousePosition.y},
            game, &pieces, &programs, camera, elapsedTime
        )
      );

      selecting = false;
    }

    // Perform the chess rules
    try{
      game->perform();
    } catch(const std::exception& e){
      std::cerr << e.what() << std::endl;

      glfwSetWindowShouldClose(window, GL_TRUE);
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
            config.user_smoke_color :
            config.ai_smoke_color,
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
          Vector4f(1.0, 1.0, 1.0, 1.0),
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
    camera->update();

    // Create the shadowMap
    shadowMapping->renderShadowMap(
      game, &pieces, &programs, &light, elapsedTime);

    // Do the cel-shading rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(
      config.background_color.x, config.background_color.y,
      config.background_color.z, config.background_color.w
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, width, height);

    // Display all pieces on the screen using the cel-shading effect
    celShadingRender(
      game, physicsWorld, &pieces, &programs, shadowMapping, camera, &light,
      &config, elapsedTime);

    // Display smoke particles
    smokeGenerator->draw(camera);

    // Swap front and back buffers
    glfwSwapBuffers(window);
  }

  glfwTerminate();

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
    DirectionalLight* light,
    Config* config,
    float elapsedTime){
  // The movement Matrix
  std::vector<GLfloat> movementMatrix;
  Vector3f translation;
  Vector3f rotation = {0, 0, 1};

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
      // If it's part of the suggested move
      if((game->suggestedUserMoveStartPosition.x == x and
          game->suggestedUserMoveStartPosition.y == y) or
          (game->suggestedUserMoveEndPosition.x == x and
          game->suggestedUserMoveEndPosition.y == y)){
        translation.z = 0.5 + 0.5 * sin(2*elapsedTime - M_PI/2.0);
      }
      movementMatrix = translate(&movementMatrix, translation);
      blackBorderProgram->setMoveMatrix(&movementMatrix);

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
  celShadingProgram->setVector3f("lightDirection", light->direction);

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
      celShadingProgram->setVector4f("color", config->user_pieces_color) :
      celShadingProgram->setVector4f("color", config->ai_pieces_color);

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

      // Draw the checkerboard
      (x + y) % 2 == 0 ?
        celShadingProgram->setVector4f("color", config->board_color_1) :
        celShadingProgram->setVector4f("color", config->board_color_2);
      if((game->selectedPiecePosition.x == x and
          game->selectedPiecePosition.y == y) or
          game->allowedNextPositions[x][y]){
        celShadingProgram->setVector4f("color", config->allowed_move_color);
      }

      // Translate the piece
      translation = {(float)(x * 4.0 - 14.0), (float)(y * 4.0 - 14.0), 0.0};
      // If it's part of the suggested move
      if((game->suggestedUserMoveStartPosition.x == x and
          game->suggestedUserMoveStartPosition.y == y) or
          (game->suggestedUserMoveEndPosition.x == x and
          game->suggestedUserMoveEndPosition.y == y)){
        translation.z = 0.5 + 0.5 * sin(2*elapsedTime - M_PI/2.0);
      }
      movementMatrix = translate(&movementMatrix, translation);
      celShadingProgram->setMoveMatrix(&movementMatrix);

      // Compute normal matrix (=inverse(transpose(movementMatrix)))
      std::vector<GLfloat> normalMatrix = inverse(&movementMatrix);
      normalMatrix = transpose(&normalMatrix);
      celShadingProgram->setNormalMatrix(&normalMatrix);

      pieces->at(BOARDCELL)->draw();

      if(piece != EMPTY){
        // Display cel-shading mesh
        piece > 0 ?
          celShadingProgram->setVector4f("color", config->user_pieces_color) :
          celShadingProgram->setVector4f("color", config->ai_pieces_color);

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
      celShadingProgram->setVector4f("color", config->user_pieces_color) :
      celShadingProgram->setVector4f("color", config->ai_pieces_color);

    pieces->at(abs(game->movingPiece))->draw();
  }
};
