#ifndef __LOGGER_H_
#define __LOGGER_H_

#include <iostream>
#include <cstdarg>
#include <string>
#include <fstream>

using namespace std;

extern fstream logfile;

typedef enum
{
	INFO	= 1,   
	WARNING  = 2,
	DEBUG = 3,
}LogLevel;	

extern LogLevel glevel;

extern string getCurrentTime();

extern void log(int arg,...);

#endif
