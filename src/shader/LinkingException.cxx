#include <string>

#include "LinkingException.hxx"

LinkingException::LinkingException(std::string msg) :
  msg{"\033[1;31mLinking error for a shader program: \033[0m" + msg}{}

LinkingException::~LinkingException() throw(){};

const char* LinkingException::what() const throw(){
  return this->msg.c_str();
};
