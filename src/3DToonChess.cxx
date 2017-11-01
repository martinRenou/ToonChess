#define GL_GLEXT_PROTOTYPES

#include <GL/glu.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <exception>
#include <map>
#include <iostream>

#include "mesh/Mesh.hxx"
#include "mesh/meshes.hxx"
#include "shader/ShaderProgram.hxx"
#include "shader/shaderPrograms.hxx"

#include "chessBoard/chessBoard.hxx"

/* Display a mesh in the 3D scene
  \param piece The piece to display, must be one of those values: KING, QUEEN,
    BISHOP, KNIGHT, ROOK, PAWN, EMPTY. If this is EMPTY, nothing will be
    displayed
  \param positionX The position of the piece on the board on the X axis
  \param positionY The position of the piece on the board on the Y axis
  \param meshes The map of meshes
  \param programs The map of shader programs
*/
void displayPiece(int piece, int positionX, int positionY,
  std::map<int, Mesh*>* meshes, std::map<int, ShaderProgram*>* programs);
GLuint pieceColor;

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
  gluPerspective(50, (double)width/height, 1, 1000);

  // Load and compile shaders
  std::map<int, ShaderProgram*> programs;
  try{
    programs = initPrograms();
  } catch(const std::exception& e){
    std::cerr << e.what() << "\n";
    return 1;
  }
  pieceColor = glGetUniformLocation(
    programs.at(CEL_SHADING)->id, "pieceColor");

  // Load meshes
  std::map<int, Mesh*> meshes = initMeshes();

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
        displayPiece(board[x][y], x, y, &meshes, &programs);
      }
    }

    glFlush();

    window.display();
  }

  deleteMeshes(&meshes);
  deletePrograms(&programs);

  return 0;
}

void displayPiece(int piece, int positionX, int positionY,
    std::map<int, Mesh*>* meshes, std::map<int, ShaderProgram*>* programs){
  if(piece == EMPTY) return;

  // Get mesh object
  Mesh* mesh = meshes->at(abs(piece));

  glPushMatrix();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(4 * positionX - 16, 4 * positionY - 16, 0);
  piece > 0 ?
    glRotatef(-90, 0, 0, 1) :
    glRotatef(90, 0, 0, 1);

  // Display black borders
  glUseProgram(programs->at(BLACK_BORDER)->id);
  glCullFace(GL_FRONT);
  mesh->draw();

  // Display cel-shading mesh
  glUseProgram(programs->at(CEL_SHADING)->id);
  piece > 0 ?
    glUniform4f(
      pieceColor,
      1.0, 0.93, 0.70, 1.0) :
    glUniform4f(
      pieceColor,
      0.51, 0.08, 0.08, 1.0);
  glCullFace(GL_BACK);
  mesh->draw();

  glPopMatrix();
};
