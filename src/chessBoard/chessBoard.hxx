#ifndef CHESSBOARD_HXX_
#define CHESSBOARD_HXX_

#include <vector>
#include <string>

/* Conversion function: converts a position in UCI format (e.g. "h3") into a
list of integers
  \param position The position in the UCI format
  \return the vector containing the x and y position on the board
*/
std::vector<int> uciFormatToPosition(std::string position);

/* Conversion function: converts a position specified with x and y into a UCI
format (e.g. "a5")
  \param positionX The position of the piece on the x axis
  \param positionY The position of the piece on the y axis
  \return the position in the UCI format
*/
std::string positionToUciFormat(int positionX, int positionY);

/* Move function
  \param movement The movement in the UCI format: "e2e3"
*/
void move(std::string movement, int board[8][8]);

#endif
