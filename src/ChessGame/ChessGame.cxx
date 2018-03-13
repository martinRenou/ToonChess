#include "StockfishConnector.hxx"
#include "GameException.hxx"

#include "ChessGame.hxx"


ChessGame::ChessGame(){
  // Start communication with stockfish
  stockfishConnector = new StockfishConnector();

  lastUserMove = "";
  clock = new sf::Clock();
};

void ChessGame::start(){
  stockfishConnector->startCommunication();
}

sf::Vector2i ChessGame::uciFormatToPosition(std::string position){
  int x(0), y(0);
  bool found(false);
  for(x = 0; x < 8 and not found; x++){
    for(y = 0; y < 8 and not found; y++){
      if(uciGrid[x][y].compare(position) == 0){
        found = true;
      }
    }
  }

  if(not found) throw GameException("Oups, something went wrong...");

  sf::Vector2i outPosition = {x - 1, y - 1};
  return outPosition;
};

std::string ChessGame::positionToUciFormat(sf::Vector2i position){
  return uciGrid[position.x][position.y];
};

const int ChessGame::boardAt(int x, int y){
  if(0 <= x and x < 8 and 0 <= y and y < 8){
    return board[x][y];
  }else{
    return OUT_OF_BOUND;
  }
};

void ChessGame::computePAWNNextPositions(sf::Vector2i position){
  // Moving one tile
  if(boardAt(position.x, position.y + 1) == EMPTY)
    allowedNextPositions[position.x][position.y + 1] = true;

  // Moving two tiles
  if(position.y == 1 and
      boardAt(position.x, position.y + 1) == EMPTY and
      boardAt(position.x, position.y + 2) == EMPTY)
    allowedNextPositions[position.x][position.y + 2] = true;

  // Moving in diagonal
  if(boardAt(position.x - 1, position.y + 1) < 0)
    allowedNextPositions[position.x - 1][position.y + 1] = true;
  if(boardAt(position.x + 1, position.y + 1) < 0)
    allowedNextPositions[position.x + 1][position.y + 1] = true;
};

void ChessGame::computeROOKNextPositions(sf::Vector2i position){
  int dx, dy;
  // Moving forward
  for(dy = 1; position.y + dy < 8; dy++){
    if(boardAt(position.x, position.y + dy) <= 0)
      allowedNextPositions[position.x][position.y + dy] = true;
    if(boardAt(position.x, position.y + dy) != EMPTY)
      break;
  }

  // Moving backward
  for(dy = 1; position.y - dy >= 0; dy++){
    if(boardAt(position.x, position.y - dy) <= 0)
      allowedNextPositions[position.x][position.y - dy] = true;
    if(boardAt(position.x, position.y - dy) != EMPTY)
      break;
  }

  // Moving to the right
  for(dx = 1; position.x + dx < 8; dx++){
    if(boardAt(position.x + dx, position.y) <= 0)
      allowedNextPositions[position.x + dx][position.y] = true;
    if(boardAt(position.x + dx, position.y) != EMPTY)
      break;
  }

  // Moving to the left
  for(dx = 1; position.x - dx >= 0; dx++){
    if(boardAt(position.x - dx, position.y) <= 0)
      allowedNextPositions[position.x - dx][position.y] = true;
    if(boardAt(position.x - dx, position.y) != EMPTY)
      break;
  }
};

void ChessGame::computeKNIGHTNextPositions(sf::Vector2i position){
  // Moving forward/right
  if(boardAt(position.x + 1, position.y + 2) <= 0)
    allowedNextPositions[position.x + 1][position.y + 2] = true;
  if(boardAt(position.x + 2, position.y + 1) <= 0)
    allowedNextPositions[position.x + 2][position.y + 1] = true;

  // Moving forward/left
  if(boardAt(position.x - 1, position.y + 2) <= 0)
    allowedNextPositions[position.x - 1][position.y + 2] = true;
  if(boardAt(position.x - 2, position.y + 1) <= 0)
    allowedNextPositions[position.x - 2][position.y + 1] = true;

  // Moving backward/right
  if(boardAt(position.x + 1, position.y - 2) <= 0)
    allowedNextPositions[position.x + 1][position.y - 2] = true;
  if(boardAt(position.x + 2, position.y - 1) <= 0)
    allowedNextPositions[position.x + 2][position.y - 1] = true;

  // Moving backward/left
  if(boardAt(position.x - 1, position.y - 2) <= 0)
    allowedNextPositions[position.x - 1][position.y - 2] = true;
  if(boardAt(position.x - 2, position.y - 1) <= 0)
    allowedNextPositions[position.x - 2][position.y - 1] = true;
};

void ChessGame::computeBISHOPNextPositions(sf::Vector2i position){
  int dd;
  // Moving forward/right
  for(dd = 1; position.x + dd < 8 and position.y + dd < 8; dd++){
    if(boardAt(position.x + dd, position.y + dd) <= 0)
      allowedNextPositions[position.x + dd][position.y + dd] = true;
    if(boardAt(position.x + dd, position.y + dd) != EMPTY)
      break;
  }

  // Moving forward/left
  for(dd = 1; position.x - dd >= 0 and position.y + dd < 8; dd++){
    if(boardAt(position.x - dd, position.y + dd) <= 0)
      allowedNextPositions[position.x - dd][position.y + dd] = true;
    if(boardAt(position.x - dd, position.y + dd) != EMPTY)
      break;
  }

  // Moving backward/right
  for(dd = 1; position.x + dd < 8 and position.y - dd < 8; dd++){
    if(boardAt(position.x + dd, position.y - dd) <= 0)
      allowedNextPositions[position.x + dd][position.y - dd] = true;
    if(boardAt(position.x + dd, position.y - dd) != EMPTY)
      break;
  }

  // Moving backward/left
  for(dd = 1; position.x - dd >= 0 and position.y - dd < 8; dd++){
    if(boardAt(position.x - dd, position.y - dd) <= 0)
      allowedNextPositions[position.x - dd][position.y - dd] = true;
    if(boardAt(position.x - dd, position.y - dd) != EMPTY)
      break;
  }
};

void ChessGame::computeKINGNextPositions(sf::Vector2i position){
  // Move forward forward/right forward/left
  if(boardAt(position.x, position.y + 1) <= 0)
    allowedNextPositions[position.x][position.y + 1] = true;
  if(boardAt(position.x + 1, position.y + 1) <= 0)
    allowedNextPositions[position.x + 1][position.y + 1] = true;
  if(boardAt(position.x - 1, position.y + 1) <= 0)
    allowedNextPositions[position.x - 1][position.y + 1] = true;

  // Move right left
  if(boardAt(position.x + 1, position.y) <= 0)
    allowedNextPositions[position.x + 1][position.y] = true;
  if(boardAt(position.x - 1, position.y + 1) <= 0)
    allowedNextPositions[position.x - 1][position.y] = true;

  // Move forward forward/right forward/left
  if(boardAt(position.x, position.y - 1) <= 0)
    allowedNextPositions[position.x][position.y - 1] = true;
  if(boardAt(position.x + 1, position.y - 1) <= 0)
    allowedNextPositions[position.x + 1][position.y - 1] = true;
  if(boardAt(position.x - 1, position.y - 1) <= 0)
    allowedNextPositions[position.x - 1][position.y - 1] = true;
};

void ChessGame::computeAllowedNextPositions(){
  // Reset matrix
  resetAllowedNextPositions();

  // Get the selected piece
  sf::Vector2i piecePosition = selectedPiecePosition;
  const int piece = boardAt(piecePosition.x, piecePosition.y);

  // If the new selected piece position is {-1, -1} which means that nothing is
  // selected, or if the new selected piece is one of AI's pieces (< 0), or if
  // the selected position corresponds to an EMPTY space (== 0), we only reset
  // the allowedNextPositions matrix
  if((piecePosition.x == -1 and piecePosition.y == -1) or
      piece < 0 or
      piece == EMPTY){
    return;
  }

  // In other cases, one user's piece has been selected, we compute the new
  // matrix according to this piece
  switch (piece) {
    case PAWN:
      computePAWNNextPositions(piecePosition);
      break;
    case ROOK:
      computeROOKNextPositions(piecePosition);
      break;
    case KNIGHT:
      computeKNIGHTNextPositions(piecePosition);
      break;
    case BISHOP:
      computeBISHOPNextPositions(piecePosition);
      break;
    case QUEEN:
      // The rule for the QUEEN is a combination of the rule for the BISHOP and
      // ROOK
      computeBISHOPNextPositions(piecePosition);
      computeROOKNextPositions(piecePosition);
      break;
    case KING:
      computeKINGNextPositions(piecePosition);
      break;
  }
};

void ChessGame::resetAllowedNextPositions(){
  for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
      allowedNextPositions[x][y] = false;
};

void ChessGame::setNewSelectedPiecePosition(
    sf::Vector2i newSelectedPiecePosition){
  if(state == USER_TURN){
    // Register last user clicked position
    oldSelectedPiecePosition = selectedPiecePosition;

    // And set the new selected piece position
    selectedPiecePosition = newSelectedPiecePosition;

    // If the new selected piece is an allowed move, it surely means that the user
    // wants to move a piece: it will be performed at the next "perform" method
    // call
    if(allowedNextPositions[selectedPiecePosition.x]
                           [selectedPiecePosition.y] == true){
      return;
    }

    // In other cases, compute the new allowedNextPositions matrix
    computeAllowedNextPositions();
  }
};

void ChessGame::perform(){
  if(state == USER_TURN) {
    // If the selected position is an allowed move, it surely means that the
    // user wants to move a piece
    if(allowedNextPositions[selectedPiecePosition.x]
                           [selectedPiecePosition.y] == true){
      // Set the currently moving piece
      movingPiece = boardAt(
        oldSelectedPiecePosition.x, oldSelectedPiecePosition.y);
      movingPieceStartPosition = oldSelectedPiecePosition;
      movingPieceEndPosition = selectedPiecePosition;
      movingPiecePosition = {
        (float)movingPieceStartPosition.x, (float)movingPieceStartPosition.y
      };

      // Remove the piece from its old position
      board[oldSelectedPiecePosition.x][oldSelectedPiecePosition.y] = EMPTY;

      // Store the last user move as an UCI string
      lastUserMove.clear();
      lastUserMove.append(
        positionToUciFormat(oldSelectedPiecePosition));
      lastUserMove.append(
        positionToUciFormat(selectedPiecePosition));

      // Unselect piece
      oldSelectedPiecePosition = {-1, -1};
      selectedPiecePosition = {-1, -1};

      // Reset allowedNextPositions matrix
      resetAllowedNextPositions();

      // Transition to the next state
      state = USER_MOVING;
      clock->restart();
    }
  }
  else if(state == USER_MOVING or state == AI_MOVING) {
    float elapsedTime = clock->getElapsedTime().asSeconds();
    if(elapsedTime < 1.0){
      movingPiecePosition = {
        elapsedTime * movingPieceEndPosition.x + (1 - elapsedTime) * movingPieceStartPosition.x,
        elapsedTime * movingPieceEndPosition.y + (1 - elapsedTime) * movingPieceStartPosition.y
      };
    } else {
      // Add the moving piece to its end position
      board[movingPieceEndPosition.x][movingPieceEndPosition.y] = movingPiece;

      // Reset attributes
      movingPiece = EMPTY;
      movingPiecePosition = {-1, -1};
      movingPieceStartPosition = {-1, -1};
      movingPieceEndPosition = {-1, -1};

      // Transition to waiting state if it's the next turn is the AI turn, USER_TURN otherwise.
      state = state == USER_MOVING ? WAITING : USER_TURN;
      clock->restart();
    }
  }
  else if(state == WAITING) {
    if(clock->getElapsedTime().asSeconds() >= 1.0){
      state = AI_TURN;
      clock->restart();
    }
  }
  else if(state == AI_TURN) {
    // Get AI decision according to the last user move
    std::string aiMove = stockfishConnector->getNextAIMove(
      lastUserMove);

    // If the AI tried to move one user's pawn, stop the game
    sf::Vector2i aiMoveStartPosition = uciFormatToPosition(
      aiMove.substr(0, 2));
    if(boardAt(aiMoveStartPosition.x, aiMoveStartPosition.y) >= 0){
      throw GameException("A forbiden move has been performed!");
    }

    // Set the currently moving piece
    movingPiece = boardAt(aiMoveStartPosition.x, aiMoveStartPosition.y);
    movingPieceStartPosition = aiMoveStartPosition;
    movingPieceEndPosition = uciFormatToPosition(
      aiMove.substr(2, 2));;
    movingPiecePosition = {
      (float)aiMoveStartPosition.x, (float)aiMoveStartPosition.y
    };

    // Remove the piece from its old position
    board[aiMoveStartPosition.x][aiMoveStartPosition.y] = EMPTY;

    // Get suggested user next move if available
    if(stockfishConnector->suggestedUserMove.compare("(none)") != 0){
      std::string startPosition_str = \
        stockfishConnector->suggestedUserMove.substr(0, 2);
      std::string endPosition_str = \
        stockfishConnector->suggestedUserMove.substr(2, 2);

      suggestedUserMoveStartPosition = uciFormatToPosition(startPosition_str);
      suggestedUserMoveEndPosition = uciFormatToPosition(endPosition_str);
    }else{
      suggestedUserMoveStartPosition = {-1, -1};
      suggestedUserMoveEndPosition = {-1, -1};
    }

    // Transition to AI_MOVING state
    state = AI_MOVING;
    clock->restart();
  }
};

ChessGame::~ChessGame(){
  delete stockfishConnector;
  delete clock;
};
