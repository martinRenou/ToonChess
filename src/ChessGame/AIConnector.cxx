#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <string.h>

#include "../utils/utils.hxx"

#include "AIConnector.hxx"

/* Read a complete line in a pipe and return it as a string
  \param readPipe FILE object in which you want to read a line
  \param print True if you want to print the result in stdin, false otherwise
  \return The complete line in a string object
*/
std::string readLine(FILE* readPipe, bool print){
  std::string line = "";
  char buffer[32] = "";

  do{
    // Clear buffer
    memset(buffer, 0, sizeof buffer);

    // Write in buffer
    fgets(buffer, sizeof buffer, readPipe);

    // Append to the string result
    line.append(buffer);

    // And repeat this task until the buffer reaches the end of line
  }while(buffer[strlen(buffer) - 1] != '\n');

  // Print the result
  if(print) std::cout << line;

  return line;
};

/* Write a complete line in a pipe
  \param writePipe FILE object in which you want to write a line
  \param print True if you want to print the line in stdin, false otherwise
*/
void writeLine(FILE* writePipe, std::string line, bool print){
  int fd = fileno(writePipe);

  write(fd, line.c_str(), line.size());

  // Print the line
  if(print) std::cout << line;
};

AIConnector::AIConnector(const std::string& ai, const int& difficulty)
    : moves{""}, ai{ai}, difficultyLevel{difficulty}{
  parentWritePipeF = NULL;
  parentReadPipeF = NULL;
};

void AIConnector::startCommunication(){
  const char* readMode = "r";
  const char* writeMode = "w";

  int fd[2];
  if(pipe(fd) == -1){
    throw std::runtime_error("AI connection error: failed to create pipes");
  }

  int childReadPipe   = fd[0];
  int parentWritePipe = fd[1];

  if(pipe(fd) == -1){
    throw std::runtime_error("AI connection error: failed to create pipes");
  }

  int parentReadPipe = fd[0];
  int childWritePipe = fd[1];

  // Create a fork of the process, the child process runs AI while the
  // parent process runs the 3D view
  pid_t pid = fork();

  // If there is an error creating child process
  if(pid < 0){
    throw std::runtime_error("AI connection error: failed to fork process");
  }

  // In the child process running AI
  if(pid == 0){
    // Redirect stdin to child read pipe and stdout to child write pipe
    dup2(childReadPipe, fileno(stdin));
    dup2(childWritePipe, fileno(stdout));

    close(parentReadPipe);
    close(parentWritePipe);
    close(childReadPipe);

    // Run AI
    execlp(ai.c_str(), ai.c_str(), (char *)NULL);

    // If everything went fine, this code shouldn't be reached
    writeLine(fdopen(childWritePipe, writeMode), "stop\n", true);
    close(childWritePipe);
    throw std::runtime_error("AI connection error: could not run the AI, please be sure \"" + ai + "\" is installed or in the path");
  }

  // In the parent process running the GUI
  close(childReadPipe);
  close(childWritePipe);

  // Get file from file descriptor
  parentReadPipeF = fdopen(parentReadPipe, readMode);
  parentWritePipeF = fdopen(parentWritePipe, writeMode);

  std::string line;

  // Check that the AI properly started
  readLine(parentReadPipeF, true);

  // Set the difficulty option
  std::string difficultyOption = "setoption name Skill Level value ";
  difficultyOption.append(std::to_string(difficultyLevel));
  difficultyOption.append("\n");
  writeLine(parentWritePipeF, difficultyOption, true);

  // Say to AI that we are ready
  writeLine(parentWritePipeF, "isready\n", true);

  // Wait for AI answer
  line = readLine(parentReadPipeF, true);
  if(line.compare("readyok\n") != 0) throw std::runtime_error("AI connection error: AI not ready, closing");
}

std::string AIConnector::getNextAIMove(std::string userMove){
  std::string line;
  std::vector<std::string> splittedLine;

  // Print user move in stdout
  std::cout << std::endl << "User move: " << userMove << std::endl;

  // Append the user move to moves
  moves.append(userMove);
  moves.append(" ");

  // Send message to AI
  line = "position startpos moves ";
  line.append(moves);
  line.append("\ngo\n");
  writeLine(parentWritePipeF, line, false);

  // Get answer
  while(true){
    line = readLine(parentReadPipeF, false);

    // Check if AI took a decision
    splittedLine = split(line, ' ');
    if(splittedLine.at(0).compare("bestmove") == 0) break;
  }

  // Get AI decision and append to moves
  std::string aiMove = splittedLine.at(1);
  // Remove '\n' if there is one
  aiMove.erase(std::remove(aiMove.begin(), aiMove.end(), '\n'), aiMove.end());
  moves.append(aiMove);
  moves.append(" ");

  // Print AI move in stdout
  std::cout << "AI move: " << aiMove << std::endl;

  // Get suggested next user move if available
  if(splittedLine.size() == 4){
    suggestedUserMove = splittedLine.at(3);

    // Print it
    std::cout << "Suggested user move: " << splittedLine.at(3);
  }else{
    suggestedUserMove = "(none)";
  }

  return aiMove;
}

AIConnector::~AIConnector(){
  // Say to AI that we are closing
  writeLine(parentWritePipeF, "quit\n", true);

  // Wait for the child process to die properly
  int status = 0;
  while((wait(&status)) > 0);

  int parentReadPipe = fileno(parentReadPipeF);
  int parentWritePipe = fileno(parentWritePipeF);

  fclose(parentReadPipeF);
  fclose(parentWritePipeF);

  close(parentReadPipe);
  close(parentWritePipe);
}
