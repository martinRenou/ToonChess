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
  int piece = this->board[lastPosition.x][lastPosition.y];

  this->board[lastPosition.x][lastPosition.y] = EMPTY;
  this->board[newPosition.x][newPosition.y] = piece;
};

void ChessGame::movePiece(std::string movement){
  std::string lastPosition_str = movement.substr(0, 2);
  std::string newPosition_str = movement.substr(2, 2);

  sf::Vector2i lastPosition = uciFormatToPosition(lastPosition_str);
  sf::Vector2i newPosition = uciFormatToPosition(newPosition_str);

  movePiece(lastPosition, newPosition);
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

void ChessGame::computeAllowedNextPositions(){
  // Reset matrix
  this->resetAllowedNextPositions();

  // Get the selected piece
  sf::Vector2i piecePosition = this->selectedPiecePosition;
  const int piece = this->board[piecePosition.x][piecePosition.y];

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
  int dx, dy;
  switch (piece) {
    case PAWN:
      // Moving one tile
      if(this->board[piecePosition.x][piecePosition.y + 1] == EMPTY)
        this->allowedNextPositions[piecePosition.x][piecePosition.y + 1] = true;

      // Moving two tiles
      if(piecePosition.y == 1 and
          this->board[piecePosition.x][piecePosition.y + 1] == EMPTY and
          this->board[piecePosition.x][piecePosition.y + 2] == EMPTY)
        this->allowedNextPositions[piecePosition.x][piecePosition.y + 2] = true;

      // Moving in diagonal
      if(this->board[piecePosition.x - 1][piecePosition.y + 1] < 0)
        this->allowedNextPositions[piecePosition.x - 1][piecePosition.y + 1] = \
          true;
      if(this->board[piecePosition.x + 1][piecePosition.y + 1] < 0)
        this->allowedNextPositions[piecePosition.x + 1][piecePosition.y + 1] = \
          true;

      break;
    case ROOK:
      // Moving forward
      for(dy = 1; piecePosition.y + dy < 8; dy++){
        if(this->board[piecePosition.x][piecePosition.y + dy] <= 0){
          this->allowedNextPositions[piecePosition.x][piecePosition.y + dy] = \
            true;
        }
        if(this->board[piecePosition.x][piecePosition.y + dy] != EMPTY){
          break;
        }
      }

      // Moving backward
      for(dy = 1; piecePosition.y - dy >= 0; dy++){
        if(this->board[piecePosition.x][piecePosition.y - dy] <= 0){
          this->allowedNextPositions[piecePosition.x][piecePosition.y - dy] = \
            true;
        }
        if(this->board[piecePosition.x][piecePosition.y - dy] != EMPTY){
          break;
        }
      }

      // Moving to the right
      for(dx = 1; piecePosition.x + dx < 8; dx++){
        if(this->board[piecePosition.x + dx][piecePosition.y] <= 0){
          this->allowedNextPositions[piecePosition.x + dx][piecePosition.y] = \
            true;
        }
        if(this->board[piecePosition.x + dx][piecePosition.y] != EMPTY){
          break;
        }
      }

      // Moving to the left
      for(dx = 1; piecePosition.x - dx >= 0; dx++){
        if(this->board[piecePosition.x - dx][piecePosition.y] <= 0){
          this->allowedNextPositions[piecePosition.x - dx][piecePosition.y] = \
            true;
        }
        if(this->board[piecePosition.x - dx][piecePosition.y] != EMPTY){
          break;
        }
      }

      break;
  }
};

void ChessGame::resetAllowedNextPositions(){
  for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
      this->allowedNextPositions[x][y] = false;
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
      if(this->board[iaMoveStartPosition.x][iaMoveStartPosition.y] >= 0){
        throw GameException("A forbiden move has been performed!");
      }

      movePiece(iaMove);

      // Transition to USER_TURN state
      this->state = USER_TURN;
      break;
    }
  }
};

ChessGame::~ChessGame(){
  delete this->stockfishConnector;
};
