#ifndef TRACEROOT_H
#define TRACEROOT_H
#include <string>
#include <iostream>
#include <exception>

class traceroot_exception: public std::exception
{
private:
    std::string what_str;
public:
    traceroot_exception(std::string what) : what_str(what){}
    virtual ~traceroot_exception() throw(){}
    virtual const char* what() const throw()
    {
      return what_str.c_str();
    }
};

// class for no reason
class traceroot
{
private:
    static const int MAX_TTL = 30;
    static const int MAX_TIMEOUT = 20; //Seconds
private:
    traceroot();
    static std::string do_hop(int hops, const std::string &dest_ip);
public:
    static void print(std::ostream &out, const std::string &dest_ip);
};

#endif // TRACEROOT_H
