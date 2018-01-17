#include "StockfishConnector.hxx"
#include "GameException.hxx"

#include "ChessGame.hxx"


ChessGame::ChessGame(){
  // Start communication with stockfish
  this->stockfishConnector = new StockfishConnector();

  try{
    this->stockfishConnector->startCommunication();
  } catch(const std::exception& e){
    std::cerr << e.what() << std::endl;

    delete this->stockfishConnector;

    throw;
  }

  this->lastUserMove = "";
  this->clock = new sf::Clock();
};

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

  if(not found) throw GameException("The IA is drunk... Can't do anything");

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
};

void ChessGame::perform(){
  switch (this->state) {
    case USER_TURN: {
      int piece = this->board
        [this->oldSelectedPiecePosition.x][this->oldSelectedPiecePosition.y];

      // Check if the user wants to move a piece
      if(this->oldSelectedPiecePosition.x != -1
          and this->selectedPiecePosition.x != -1
          and piece > 0){

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
