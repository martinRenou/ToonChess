#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "chessBoard.hxx"

#include "../constants.hxx"
#include "../mesh/meshes.hxx"

std::string uciGrid[8][8] = {
  "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8",
  "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8",
  "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8",
  "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8",
  "e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8",
  "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8",
  "g1", "g2", "g3", "g4", "g5", "g6", "g7", "g8",
  "h1", "h2", "h3", "h4", "h5", "h6", "h7", "h8",
};

sf::Vector2i uciFormatToPosition(std::string position){
  int x(0), y(0);
  bool found(false);
  for(x = 0; x < 8 and not found; x++){
    for(y = 0; y < 8 and not found; y++){
      if(uciGrid[x][y].compare(position) == 0){
        found = true;
      }
    }
  }

  //TODO: Throw an exception if not found and close program properly

  sf::Vector2i outPosition = {x - 1, y - 1};
  return outPosition;
};

std::string positionToUciFormat(sf::Vector2i position){
  return uciGrid[position.x][position.y];
};

void movePiece(
    sf::Vector2i lastPosition, sf::Vector2i newPosition, int board[8][8]){
  int piece = board[lastPosition.x][lastPosition.y];

  board[lastPosition.x][lastPosition.y] = EMPTY;
  board[newPosition.x][newPosition.y] = piece;
};

std::string getMovement(sf::Vector2i lastPosition, sf::Vector2i newPosition){
  std::string movement = "";

  movement.append(positionToUciFormat(lastPosition));
  movement.append(positionToUciFormat(newPosition));

  return movement;
}

void movePiece(std::string movement, int board[8][8]){
  std::string lastPosition_str = movement.substr(0, 2);
  std::string newPosition_str = movement.substr(2, 2);

  sf::Vector2i lastPosition = uciFormatToPosition(lastPosition_str);
  sf::Vector2i newPosition = uciFormatToPosition(newPosition_str);

  movePiece(lastPosition, newPosition, board);
};
