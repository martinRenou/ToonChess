#ifndef CHESSGAME_HXX_
#define CHESSGAME_HXX_

#include <string>

#include <SFML/Graphics.hpp>

#include "../constants.hxx"
#include "StockfishConnector.hxx"


// cppcheck-suppress noCopyConstructor
class ChessGame {
private:
  /* Last user move */
  std::string lastUserMove;

  /* The connector with Stockfish */
  StockfishConnector* stockfishConnector;

  /* The state of the game, should be USER_TURN, IA_TURN or WAITING */
  int state = USER_TURN;

  /* Clock used for waiting between the USER_TURN and the IA_TURN */
  sf::Clock* clock;

  /* Grid used for conversions between position and UCI format */
  const std::string uciGrid[8][8] = {
    {"a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8"},
    {"b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8"},
    {"c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8"},
    {"d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8"},
    {"e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8"},
    {"f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8"},
    {"g1", "g2", "g3", "g4", "g5", "g6", "g7", "g8"},
    {"h1", "h2", "h3", "h4", "h5", "h6", "h7", "h8"},
  };

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

  /* Move function, this will move a chess piece on the board
    \param lastPosition The last position of the piece to move
    \param newPosition The new position of the piece to move
  */
  void movePiece(sf::Vector2i lastPosition, sf::Vector2i newPosition);

  /* Move function, this will move a chess piece on the board
    \param movement The movement in the UCI format "a2a4"
  */
  void movePiece(std::string movement);

  /* Method used for accessing piece at position {x, y}, if {x, y} doesn't
  correspond to a position on the board, it returns OUT_OF_BOUND constant and
  doesn't throw exception */
  const int boardAt(int x, int y);

  /* Compute the allowedNextPositions matrix for a specific piece */
  void computePAWNNextPositions(sf::Vector2i position);
  void computeROOKNextPositions(sf::Vector2i position);
  void computeKNIGHTNextPositions(sf::Vector2i position);
  void computeBISHOPNextPositions(sf::Vector2i position);
  void computeKINGNextPositions(sf::Vector2i position);

  /* Compute the allowedNextPositions matrix according to the selected piece */
  void computeAllowedNextPositions();

  /* Reset the allowedNextPositions matrix with its default value */
  void resetAllowedNextPositions();

public:
  /* Constructor */
  ChessGame();

  /* The checkerboard */
  int board[8][8] = {
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {QUEEN, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*QUEEN},
    {KING, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KING},
    {BISHOP, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*BISHOP},
    {KNIGHT, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*KNIGHT},
    {ROOK, PAWN, EMPTY, EMPTY, EMPTY, EMPTY, AI*PAWN, AI*ROOK}
  };

  /* The allowed next positions for the currently selected piece */
  bool allowedNextPositions[8][8] = {
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false}
  };

  /* Position of the currently selected chess piece {-1, -1} if nothing is
  selected */
  sf::Vector2i selectedPiecePosition = {-1, -1};

  /* Position of the selected chess piece just before the currently selected
  one was selected */
  sf::Vector2i oldSelectedPiecePosition = {-1, -1};

  /* Start position of the suggested user move, {-1, -1} if nothing is suggested
  */
  sf::Vector2i suggestedUserMoveStartPosition = {-1, -1};

  /* End position of the suggested user move, {-1, -1} if nothing is suggested
  */
  sf::Vector2i suggestedUserMoveEndPosition = {-1, -1};

  /* Start the game engine
    \throw ConnectionException if communication with Stockfish didn't start
    properly
  */
  void start();

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
  ~ChessGame();
};

#endif
