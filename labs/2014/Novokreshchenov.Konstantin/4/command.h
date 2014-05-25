#ifndef COMMAND_H
#define COMMAND_H

#include "manager.h"
#include <vector>

class Command;
class CommandAlloc;
class CommandFree;
class CommandInfo;
class CommandMap;

typedef std::vector<std::string> vecStr;

class Parser
{
    bool parse_alloc (vecStr const & vCmd);
    bool parse_free (vecStr const & vCmd);
    bool parse_info (vecStr const & vCmd);
    bool parse_map (vecStr const & vCmd);

public:
    Command* parse (vecStr const & vCmd);
};

class Command
{
public:
    virtual void accept (MemoryManager & memoryManager)
    { memoryManager.execute(this); }

    virtual ~Command()
    {}
};

class CommandAlloc: public Command
{
    size_t size_;

public:
    CommandAlloc(size_t size): size_(size)
    {}

    size_t get_size () const
    { return size_; }

    virtual void accept (MemoryManager & memoryManager)
    { memoryManager.execute(this); }

    ~CommandAlloc()
    {}
};

class CommandFree: public Command
{
    size_t ptr_;

public:
    CommandFree(size_t ptr): ptr_(ptr)
    {}

    size_t get_ptr () const
    { return ptr_; }

    virtual void accept (MemoryManager & memoryManager)
    { memoryManager.execute(this); }

    ~CommandFree()
    {}
};

class CommandInfo: public Command
{
public:
    virtual void accept (MemoryManager & memoryManager)
    { memoryManager.execute(this); }

    ~CommandInfo()
    {}
};

class CommandMap: public Command
{
public:
    virtual void accept (MemoryManager & memoryManager)
    { memoryManager.execute(this); }

    ~CommandMap()
    {}
};

#endif // COMMAND_H
