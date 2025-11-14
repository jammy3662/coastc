/*
| 
|  Logging errors, warnings, and notes
| 
*/

#ifndef LOG_H
#define LOG_H

struct Location
{
	short line, column;
	char* source_file;
};

struct Log
{
	Location location;
	char* message;
	
	enum Level
	{ NOTE, WARNING, ERROR, }
	severity;
};

#endif
