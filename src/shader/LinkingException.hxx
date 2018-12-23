#ifndef LINKINGEXCEPTION_HXX_
#define LINKINGEXCEPTION_HXX_

#include <exception>
#include <string>

class LinkingException : public std::exception
{
public:

    explicit LinkingException(const std::string& msg);

    virtual ~LinkingException() throw();

    virtual const char* what() const throw();

private:

    std::string msg;
};

#endif
