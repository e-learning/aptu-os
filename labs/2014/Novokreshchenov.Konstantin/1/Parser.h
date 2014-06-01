#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

static int const NOT_MATCHED = 0;
static int const MATCHED = 1;
static int const ERROR = 2;
static int const EXIT = 3;

class CommandParser;

int print_prompt ();
void parse_input (std::string &, std::vector<std::string> &);
void create_parsers (std::vector<CommandParser*> & parsers);
void delete_parsers (std::vector<CommandParser*> & parsers);

template < typename T, typename A, template<typename, typename> class C >
void print (C<T, A> & c)
{
    for (typename C<T, A>::const_iterator it = c.begin(); it != c.end(); ++it) {
        std::cout << *it << std::endl;
    }
}


class CommandParser
{
public:
    virtual ~CommandParser(){}
    virtual int parse (std::vector<std::string> const & command) = 0;
};


class LsParser: public CommandParser
{
public:
    virtual int parse (const std::vector<std::string> &command);
};


class PwdParser: public CommandParser
{
public:
    virtual int parse (const std::vector<std::string> &command);
};


class PsParser: public CommandParser
{
public:
    virtual int parse (const std::vector<std::string> &command);
private:
    std::string get_process_name (std::string const & pid);
};


class KillParser: public CommandParser
{
public:
    KillParser();
    virtual ~KillParser(){}
    virtual int parse (std::vector<std::string> const & command);
    void print_signal_list ();
    void print_command_info ();

private:
    std::map<std::string, int> signal_map;
};


class ExitParser: public CommandParser
{
public:
    virtual int parse (const std::vector<std::string> &command);
};


#endif // PARSER_H
