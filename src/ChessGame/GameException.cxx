#include <string>

#include "GameException.hxx"

GameException::GameException(const std::string& msg) :
  msg{"\033[1;31mGame error: \033[0m" + msg}{}

GameException::~GameException() throw(){};

const char* GameException::what() const throw(){
  return msg.c_str();
};
