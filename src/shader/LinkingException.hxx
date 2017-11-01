#ifndef LINKINGEXCEPTION_HXX_
#define LINKINGEXCEPTION_HXX_

#include <exception>
#include <string>

class LinkingException : public std::exception{
  private:
    std::string msg;

  public:
    LinkingException(std::string msg);

    virtual ~LinkingException() throw();

    virtual const char* what() const throw();
};

#endif
