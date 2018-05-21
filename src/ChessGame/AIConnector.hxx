#ifndef AI_CONNECTOR_HXX
#define AI_CONNECTOR_HXX

#include <iostream>
#include <string>

#include "../constants.hxx"

class AIConnector {
private:
  std::string ai;

  /* Communication pipes between child and parent processes
    (respectively AI and the GUI)
  */
  FILE* parentWritePipeF;
  FILE* parentReadPipeF;

  /* All the moves since the beginning of the game */
  std::string moves;

  /* Game difficulty */
  int difficultyLevel = DIFFICULTY_EASY;

public:
  /* Constructor */
  AIConnector(const std::string& ai, const int& difficulty);

  /* Function which starts the AI and initialize the communication
    /throw ConnectionException if something went wrong while initializing
      connection with the AI
  */
  void startCommunication();

  /* Get the next AI move according to the last user move
    \param userMove The last user move in uci format
    \return The next AI move in uci format
  */
  std::string getNextAIMove(std::string userMove);

  /* Suggested next user move, "(none)" is nothing is suggested by the AI */
  std::string suggestedUserMove = "(none)";

  /* Destructor, this will properly stop the communication */
  ~AIConnector();
};

#endif
