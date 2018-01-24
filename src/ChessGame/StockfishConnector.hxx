#ifndef STOCKFISHCONNECTOR_HXX
#define STOCKFISHCONNECTOR_HXX

#include <iostream>
#include <string>

class StockfishConnector {
private:
  /* Communication pipes between child and parent processes
    (respectively stockfish and the GUI)
  */
  FILE* parentWritePipeF;
  FILE* parentReadPipeF;

  /* All the moves since the beginning of the game */
  std::string moves;

public:
  /* Constructor */
  StockfishConnector();

  /* Function which starts Stockfish and initialize the communication
    /throw ConnectionException if something went wrong while initializing
      connection with stockfish
  */
  void startCommunication();

  /* Get the next IA move according to the last user move
    \param userMove The last user move in uci format
    \return The next IA move in uci format
  */
  std::string getNextIAMove(std::string userMove);

  /* Suggested next user move, "(none)" is nothing is suggested by the IA */
  std::string suggestedUserMove = "(none)";

  /* Destructor, this will properly stop the communication */
  ~StockfishConnector();
};

#endif
