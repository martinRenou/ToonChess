#include <string>

#include "LinkingException.hxx"

LinkingException::LinkingException(const std::string& msg) :
  msg{"\033[1;31mLinking error for a shader program: \033[0m" + msg}{}

LinkingException::~LinkingException() throw(){};

const char* LinkingException::what() const throw(){
  return msg.c_str();
};
