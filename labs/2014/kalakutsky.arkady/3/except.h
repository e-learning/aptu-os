#ifndef EXCEPT
#define EXCEPT

#include <exception>
#include <string>

class FSException: public std::exception{
	std::string msg;
  public:
  FSException(const char* msg):msg(msg){
		
	}
	~FSException() throw(){
		
	}
	virtual const char* what() const throw()
	{
		return msg.c_str();
	}
	
};

class FileNotFoundException: public FSException{
  public:
  FileNotFoundException():FSException("File not found."){}
};

class BadRootPathException: public FSException{
  public:
  BadRootPathException():FSException("Bad root path. Can't read blocks."){}
};


class NotDirectoryException: public FSException{
  public:
  NotDirectoryException():FSException(
	  "Wrong path. You can't get file from directory."){}
};

class TooLongFileNameException: public FSException{
  public:
  TooLongFileNameException():FSException(
	  "Wrong path. Too long file names."){}
};


class RelativePathException: public FSException{
  public:
  RelativePathException():FSException(
	  "Wrong path. Only absolute paths accepted."){}
};

class WorkflowException: public FSException{
  public:
  WorkflowException():FSException("Programmer was stupid :P"){}
};

class AlreadyExistsException: public FSException{
  public:
  AlreadyExistsException():FSException(
	  "File already exists."){}
};

class NotEnoughMemoryException: public FSException{
  public:
  NotEnoughMemoryException():FSException(
	  "Not enough memory."){}
};

#endif
