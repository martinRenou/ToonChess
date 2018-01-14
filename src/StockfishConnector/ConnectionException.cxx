#include <string>

#include "ConnectionException.hxx"

ConnectionException::ConnectionException(const std::string& msg) :
  msg{"\033[1;31mConnection error: \033[0m\n" + msg}{}

ConnectionException::~ConnectionException() throw(){};

const char* ConnectionException::what() const throw(){
  return this->msg.c_str();
};
