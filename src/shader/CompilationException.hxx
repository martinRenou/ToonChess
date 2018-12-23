#ifndef COMPILATIONEXCEPTION_HXX_
#define COMPILATIONEXCEPTION_HXX_

#include <exception>
#include <string>

class CompilationException : public std::exception
{
public:

    explicit CompilationException(
      const std::string& shaderPath, const std::string& msg);

    virtual ~CompilationException() throw();

    virtual const char* what() const throw();

private:

    std::string msg;
};

#endif
