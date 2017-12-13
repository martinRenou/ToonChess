#ifndef GAMEINFO_HXX_
#define GAMEINFO_HXX_

#include <GL/gl.h>
#include <SFML/Graphics.hpp>

#include <vector>

#include "constants.hxx"

struct GameInfo {
  // Rendering informations
  GLboolean shadowMapping = true;
  GLfloat shadowMapResolution = SHADOWMAPPING_LOW;
  GLint antialiasingLevel = ANTIALIASING_LOW;
  GLboolean vSync = false;
  GLint fovy = 50;
  GLint width = 1024;
  GLint height = 576;

  GLfloat cameraRotationRadius = 40.0;

  // Light direction should NOT be collinear to (0, 0, 1) to prevent
  // lightLookAtMatrix computation issues, and should NOT be collinear to
  // (1, 1, 0) to prevent shadows artefacts
  sf::Vector3f lightDirection = {-1.0, 1.0, -1.0};

  // Position of the selected chess piece
  sf::Vector2i selectedPiecePosition = {-1, -1};

  // View and Projection matrices
  sf::Vector3f cameraPosition;
  std::vector<GLfloat> cameraViewMatrix;
  std::vector<GLfloat> cameraProjectionMatrix;
  std::vector<GLfloat> lightViewMatrix;
  std::vector<GLfloat> lightProjectionMatrix;

  // The board
  int board[8][8] = {
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {KING, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KING},
    {QUEEN, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*QUEEN},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK}
  };
};

#endif
