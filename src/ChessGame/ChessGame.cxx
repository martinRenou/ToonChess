#include "StockfishConnector.hxx"
#include "GameException.hxx"

#include "ChessGame.hxx"


ChessGame::ChessGame(){
  // Start communication with stockfish
  this->stockfishConnector = new StockfishConnector();

  this->lastUserMove = "";
  this->clock = new sf::Clock();
};

void ChessGame::start(){
  this->stockfishConnector->startCommunication();
}

sf::Vector2i ChessGame::uciFormatToPosition(std::string position){
  int x(0), y(0);
  bool found(false);
  for(x = 0; x < 8 and not found; x++){
    for(y = 0; y < 8 and not found; y++){
      if(this->uciGrid[x][y].compare(position) == 0){
        found = true;
      }
    }
  }

  if(not found) throw GameException("Oups, something went wrong...");

  sf::Vector2i outPosition = {x - 1, y - 1};
  return outPosition;
};

std::string ChessGame::positionToUciFormat(sf::Vector2i position){
  return this->uciGrid[position.x][position.y];
};

void ChessGame::movePiece(sf::Vector2i lastPosition, sf::Vector2i newPosition){
  int piece = board[lastPosition.x][lastPosition.y];

  board[lastPosition.x][lastPosition.y] = EMPTY;
  board[newPosition.x][newPosition.y] = piece;
};

void ChessGame::movePiece(std::string movement){
  std::string lastPosition_str = movement.substr(0, 2);
  std::string newPosition_str = movement.substr(2, 2);

  sf::Vector2i lastPosition = uciFormatToPosition(lastPosition_str);
  sf::Vector2i newPosition = uciFormatToPosition(newPosition_str);

  movePiece(lastPosition, newPosition);
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
  this->resetAllowedNextPositions();

  // Get the selected piece
  sf::Vector2i piecePosition = this->selectedPiecePosition;
  const int piece = boardAt(piecePosition.x, piecePosition.y);

  // If the new selected piece position is {-1, -1} which means that nothing is
  // selected, or if the new selected piece is one of IA's pieces (< 0), or if
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
      this->allowedNextPositions[x][y] = false;
};

void ChessGame::setNewSelectedPiecePosition(
    sf::Vector2i newSelectedPiecePosition){
  // Register last user clicked position
  this->oldSelectedPiecePosition = this->selectedPiecePosition;

  // And set the new selected piece position
  this->selectedPiecePosition = newSelectedPiecePosition;

  // If the new selected piece is an allowed move, it surely means that the user
  // wants to move a piece: it will be performed at the next "perform" method
  // call
  if(allowedNextPositions[this->selectedPiecePosition.x]
                         [this->selectedPiecePosition.y] == true){
    return;
  }

  // In other cases, compute the new allowedNextPositions matrix
  this->computeAllowedNextPositions();
};

void ChessGame::perform(){
  switch (this->state) {
    case USER_TURN: {
      // If the selected position is an allowed move, it surely means that the
      // user wants to move a piece
      if(allowedNextPositions[this->selectedPiecePosition.x]
                             [this->selectedPiecePosition.y] == true){
        // Move the piece
        movePiece(this->oldSelectedPiecePosition, this->selectedPiecePosition);

        // Store the last user move as an UCI string
        this->lastUserMove.clear();
        this->lastUserMove.append(
          positionToUciFormat(this->oldSelectedPiecePosition));
        this->lastUserMove.append(
          positionToUciFormat(this->selectedPiecePosition));

        // Unselect piece
        this->oldSelectedPiecePosition = {-1, -1};
        this->selectedPiecePosition = {-1, -1};

        // Reset allowedNextPositions matrix
        this->resetAllowedNextPositions();

        // Transition to waiting state and restart the clock for measuring
        // waiting time
        this->state = WAITING;
        this->clock->restart();
      }
      break;
    }
    case WAITING: {
      if(this->clock->getElapsedTime().asSeconds() >= 1.0)
        this->state = IA_TURN;
      break;
    }
    case IA_TURN: {
      // Get IA decision according to the last user move
      std::string iaMove = stockfishConnector->getNextIAMove(
        this->lastUserMove);

      // If the IA tried to move one user's pawn, stop the game
      sf::Vector2i iaMoveStartPosition = uciFormatToPosition(
        iaMove.substr(0, 2));
      if(boardAt(iaMoveStartPosition.x, iaMoveStartPosition.y) >= 0){
        throw GameException("A forbiden move has been performed!");
      }

      movePiece(iaMove);

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

      // Transition to USER_TURN state
      this->state = USER_TURN;
      break;
    }
  }
};

ChessGame::~ChessGame(){
  delete this->stockfishConnector;
  delete this->clock;
};
