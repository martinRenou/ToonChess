#include <string>

#include "CompilationException.hxx"

CompilationException::CompilationException(
    const std::string& shaderPath, const std::string& msg) :
  msg{"\033[1;31mCompilation error for shader \033[0m" +
    shaderPath + ":\n" + msg}{}

CompilationException::~CompilationException() throw(){};

const char* CompilationException::what() const throw(){
  return msg.c_str();
};
