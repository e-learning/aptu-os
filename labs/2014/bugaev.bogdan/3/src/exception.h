#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>


class Exception
{
public:
    Exception(std::string const &message):
        m_message(message)
    {
    }

    std::string const message() const
    {
        return m_message;
    }

private:
    std::string m_message;
};


#define CREATE_EXCEPTION(E) \
class E: public Exception \
{ \
public: \
    E(std::string const &message): \
        Exception(message) \
    { \
    } \
};


#endif // EXCEPTION_H
