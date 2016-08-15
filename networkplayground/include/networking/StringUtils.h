#ifndef StringUtils_h
#define StringUtils_h

#include <stdio.h>
#include <string>
namespace StringUtils
{
std::string GetCommandLineArg( int inIndex );

std::string Sprintf( const char* inFormat, ... );
static std::string logFile;

void initLog( std::string logFile );

void Log( const char* inFormat, ... );
void LogFile( const char* inFormat, ... );
}

#define LOG( ... ) StringUtils::Log( __VA_ARGS__ );
#endif
