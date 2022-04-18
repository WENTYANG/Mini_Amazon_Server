#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#include <exception>
#include <string>

using namespace std;

class MyException : public exception {
 public:
  MyException() : message("Error.") {}
  MyException(string str) : message("Error : " + str) {}
  ~MyException() throw() {}

  virtual const char * what() const throw() { return message.c_str(); }

 private:
  string message;
};

class VersionErrorException : public exception {
 public:
  VersionErrorException() : message("Sql transaction fail. Beacuse:  ") {}
  VersionErrorException(string str) : message("Sql transaction fail. Beacuse:  " + str) {}
  ~VersionErrorException() throw() {}

  virtual const char * what() const throw() { return message.c_str(); }

 private:
  string message;
};

#endif