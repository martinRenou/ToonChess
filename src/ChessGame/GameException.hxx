#ifndef GAMEEXCEPTION_HXX_
#define GAMEEXCEPTION_HXX_

#include <exception>
#include <string>

class GameException : public std::exception{
  private:
    std::string msg;

  public:
    explicit GameException(const std::string& msg);

    virtual ~GameException() throw();

    virtual const char* what() const throw();
};

#endif
