#include <string>

#include "CompilationException.hxx"

CompilationException::CompilationException(
    std::string shaderPath, std::string msg){
  this->msg =
    "\033[1;31mCompilation error for shader \033[0m" +
    shaderPath + ":\n" + msg;
}

CompilationException::~CompilationException() throw(){};

const char* CompilationException::what() const throw(){
  return this->msg.c_str();
};
