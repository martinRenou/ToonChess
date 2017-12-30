#ifndef STOCKFISHCONNECTOR_HXX
#define STOCKFISHCONNECTOR_HXX

#include <unistd.h>
#include <iostream>
#include <string.h>

#include "ConnectionException.hxx"

/* Creates a fork of the process, the child process runs stockfish while the
  parent process runs the 3D view
  /throw ConnectionException if the fork went wrong or if stockfish could not be
    run
*/
void startCommunication(){
  pid_t pid = fork();

  if(pid < 0){
    throw ConnectionException("Failed to fork processes");
  }
  if(pid == 0){
    // In the child process running Stockfish
    char stockfish_cmd[] = "stockfish";
    char* args[] = { stockfish_cmd, 0 };
    execvp(args[0], args);

    // If everything went fine, this code shouldn't be reached
    throw ConnectionException(
      "Could not run stockfish, please be sure it's installed");
  } else{
    // In the parent process running the GUI
  }
}

#endif
