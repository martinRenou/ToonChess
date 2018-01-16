#include "StockfishConnector.hxx"
#include "GameException.hxx"

#include "Game.hxx"

const std::string uciGrid[8][8] = {
  "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8",
  "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8",
  "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8",
  "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8",
  "e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8",
  "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8",
  "g1", "g2", "g3", "g4", "g5", "g6", "g7", "g8",
  "h1", "h2", "h3", "h4", "h5", "h6", "h7", "h8",
};

/* Conversion function: converts a position in UCI format (e.g. "h3") into a
vector
  \param position The position in the UCI format
  \return the SFML vector representing the position on the board
*/
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

/* Conversion function: converts a position specified with x and y into a UCI
format (e.g. "a5")
  \param position The position as an sf::Vector2i
  \return the position in the UCI format
*/
std::string positionToUciFormat(sf::Vector2i position){
  return uciGrid[position.x][position.y];
};

Game::Game(){
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

void Game::movePiece(sf::Vector2i lastPosition, sf::Vector2i newPosition){
  int piece = this->board[lastPosition.x][lastPosition.y];

  this->board[lastPosition.x][lastPosition.y] = EMPTY;
  this->board[newPosition.x][newPosition.y] = piece;
};

void Game::movePiece(std::string movement){
  std::string lastPosition_str = movement.substr(0, 2);
  std::string newPosition_str = movement.substr(2, 2);

  sf::Vector2i lastPosition = uciFormatToPosition(lastPosition_str);
  sf::Vector2i newPosition = uciFormatToPosition(newPosition_str);

  movePiece(lastPosition, newPosition);
};

void Game::setNewSelectedPiecePosition(sf::Vector2i newSelectedPiecePosition){
  // Register last user clicked position
  this->oldSelectedPiecePosition = this->selectedPiecePosition;

  // Get selected piece using color picking
  this->selectedPiecePosition = newSelectedPiecePosition;
};

void Game::perform(){
  switch (this->state) {
    case USER_TURN: {
      int piece = this->board
        [this->oldSelectedPiecePosition.x][this->oldSelectedPiecePosition.y];

      // If it's the user turn, check if he wants to move a piece
      if(this->state == USER_TURN
          and this->oldSelectedPiecePosition.x != -1
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

Game::~Game(){
  delete this->stockfishConnector;
};
