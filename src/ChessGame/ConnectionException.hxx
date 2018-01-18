#ifndef COMPILATIONEXCEPTION_HXX_
#define COMPILATIONEXCEPTION_HXX_

#include <exception>
#include <string>

class ConnectionException : public std::exception{
  private:
    std::string msg;

  public:
    explicit ConnectionException(const std::string& msg);

    virtual ~ConnectionException() throw();

    virtual const char* what() const throw();
};

#endif
