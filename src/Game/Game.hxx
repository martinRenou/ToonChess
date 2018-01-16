#ifndef GAME_HXX_
#define GAME_HXX_

#include <string>

#include <SFML/Graphics.hpp>

#include "../constants.hxx"
#include "StockfishConnector.hxx"

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


// cppcheck-suppress noCopyConstructor
class Game {
private:
  /* Last user move */
  std::string lastUserMove;

  /* The connector with Stockfish */
  StockfishConnector* stockfishConnector;

  /* The state of the game, should be USER_TURN, IA_TURN or WAITING */
  int state = USER_TURN;

  /* Clock used for waiting between the USER_TURN and the IA_TURN */
  sf::Clock* clock;

  /* Move function, this will move a chess piece on the board
    \param lastPosition The last position of the piece to move
    \param newPosition The new position of the piece to move
  */
  void movePiece(sf::Vector2i lastPosition, sf::Vector2i newPosition);

  /* Move function, this will move a chess piece on the board
    \param movement The movement in the UCI format "a2a4"
  */
  void movePiece(std::string movement);

public:
  /* Constructor */
  Game();

  /* The checkerboard */
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

  /* Position of the currently selected chess piece {-1, -1} if nothing is
  selected */
  sf::Vector2i selectedPiecePosition = {-1, -1};

  /* Position of the selected chess piece just before the currently selected
  one was selected */
  sf::Vector2i oldSelectedPiecePosition = {-1, -1};

  /* Set the new clicked position on the board */
  void setNewSelectedPiecePosition(sf::Vector2i newSelectedPiecePosition);

  /* Perform the chess rules depending on the game state, if it's the USER_TURN
    it will move one chess piece according to the currently clicked piece, if
    it's WAITING it will wait one second before changing to IA_TURN, if it's
    IA_TURN it will ask Stockfish what is the next IA move
    \throw GameException if chess rules are not respected
  */
  void perform();

  /* Destructor */
  ~Game();
};

#endif
