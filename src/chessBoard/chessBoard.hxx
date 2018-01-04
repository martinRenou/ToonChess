#ifndef CHESSBOARD_HXX_
#define CHESSBOARD_HXX_

#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

/* Conversion function: converts a position in UCI format (e.g. "h3") into a
vector
  \param position The position in the UCI format
  \return the SFML vector representing the position on the board
*/
sf::Vector2i uciFormatToPosition(std::string position);

/* Conversion function: converts a position specified with x and y into a UCI
format (e.g. "a5")
  \param position The position as an sf::Vector2i
  \return the position in the UCI format
*/
std::string positionToUciFormat(sf::Vector2i position);

/* Get movement as UCI format from a last position and a new position
  \param lastPosition The last position of the piece as an sf::Vector2i
  \param newPosition The new position of the piece as an sf::Vector2i
*/
std::string getMovement(sf::Vector2i lastPosition, sf::Vector2i newPosition);

/* Move function
  \param lastPosition The last position of the piece to move
  \param newPosition The new position of the piece to move
  \param board The chess board
*/
void movePiece(
  sf::Vector2i lastPosition, sf::Vector2i newPosition, int board[8][8]);

/* Move function
  \param movement The movement in the UCI format "a2a4"
  \param board The chess board
*/
void movePiece(std::string movement, int board[8][8]);

#endif
