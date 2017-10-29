#define GL_GLEXT_PROTOTYPES

#include <GL/glu.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "mesh/Mesh.hxx"

#include "shader/ShaderProgram.hxx"
#include "shader/Shader.hxx"

#include "chessBoard/chessBoard.hxx"

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

void displayPiece(int piece, int positionX, int positionY);
ShaderProgram* blackBorderShaderProgram;
ShaderProgram* celShadingShaderProgram;
Mesh* king;
Mesh* queen;
Mesh* bishop;
Mesh* knight;
Mesh* rook;
Mesh* pawn;


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
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70, (double)width/height, 1, 1000);

  // Load cel-shading shaders
  Shader* celShadingVertexShader = new Shader(
    "../shaders/celShadingVertexShader.glsl",
    GL_VERTEX_SHADER
  );

  Shader* celShadingFragmentShader = new Shader(
    "../shaders/celShadingFragmentShader.glsl",
    GL_FRAGMENT_SHADER
  );

  std::vector<Shader*> celShadingShaders = {
    celShadingVertexShader, celShadingFragmentShader};
  celShadingShaderProgram = new ShaderProgram(celShadingShaders);

  // Try to compile shaders
  bool compilationIsSuccess = celShadingShaderProgram->compile();
  if(!compilationIsSuccess){
    delete celShadingShaderProgram;

    return 1;
  }

  // Load black border shaders
  Shader* blackBorderVertexShader = new Shader(
    "../shaders/blackBorderVertexShader.glsl",
    GL_VERTEX_SHADER
  );

  Shader* blackBorderFragmentShader = new Shader(
    "../shaders/blackBorderFragmentShader.glsl",
    GL_FRAGMENT_SHADER
  );

  std::vector<Shader*> blackBorderShaders = {
    blackBorderVertexShader, blackBorderFragmentShader};
  blackBorderShaderProgram = new ShaderProgram(
    blackBorderShaders);

  // Try to compile shaders
  compilationIsSuccess = blackBorderShaderProgram->compile();
  if(!compilationIsSuccess){
    delete celShadingShaderProgram;
    delete blackBorderShaderProgram;

    return 1;
  }

  // Create and load meshes
  king = new Mesh("../assets/king.obj");
  king->initBuffers();

  queen = new Mesh("../assets/queen.obj");
  queen->initBuffers();

  bishop = new Mesh("../assets/bishop.obj");
  bishop->initBuffers();

  rook = new Mesh("../assets/rook.obj");
  rook->initBuffers();

  knight = new Mesh("../assets/knight.obj");
  knight->initBuffers();

  pawn = new Mesh("../assets/pawn.obj");
  pawn->initBuffers();

  // Render loop
  bool running = true;
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
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gluLookAt(0, -40, 20, 0, 0, 0, 0, 0, 1);

    // Display all pieces
    for(int x = 0; x < 8; x++){
      for(int y = 0; y < 8; y++){
        if(board[x][y] == EMPTY) continue;

        displayPiece(board[x][y], x, y);
      }
    }

    glFlush();

    window.display();
  }

  delete king;
  delete queen;
  delete bishop;
  delete knight;
  delete rook;
  delete pawn;
  delete celShadingShaderProgram;
  delete blackBorderShaderProgram;

  return 0;
}

void displayPiece(int piece, int positionX, int positionY){
  Mesh* mesh;
  switch (abs(piece)) {
    case KING:
      mesh = king;
      break;
    case QUEEN:
      mesh = queen;
      break;
    case BISHOP:
      mesh = bishop;
      break;
    case KNIGHT:
      mesh = knight;
      break;
    case ROOK:
      mesh = rook;
      break;
    case PAWN:
      mesh = pawn;
      break;
  }

  glPushMatrix();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(4 * positionX - 16, 4 * positionY - 16, 0);
  // glRotatef(0, 0, 0, 0);

  // Display black borders
  glUseProgram(blackBorderShaderProgram->id);
  glCullFace(GL_FRONT);
  mesh->draw();

  // Display cel-shading mesh
  glUseProgram(celShadingShaderProgram->id);
  glCullFace(GL_BACK);
  mesh->draw();

  glPopMatrix();
};
