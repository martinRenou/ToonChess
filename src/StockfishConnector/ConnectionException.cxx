#include <string>

#include "ConnectionException.hxx"

ConnectionException::ConnectionException(std::string msg) :
  msg{"\033[1;31mConnection error: \033[0m" + msg}{}

ConnectionException::~ConnectionException() throw(){};

const char* ConnectionException::what() const throw(){
  return this->msg.c_str();
};
