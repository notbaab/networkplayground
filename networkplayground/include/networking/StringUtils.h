#ifndef StringUtils_h
#define StringUtils_h

#include <stdio.h>
#include <string>
namespace Logger
{

enum LogLevel
{
    ALL = 0,
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    OFF,
};

std::string GetCommandLineArg( int inIndex );

std::string Sprintf( const char* inFormat, ... );
static std::string logFile;

static LogLevel sCurrentLevel;

void InitLog(LogLevel level, std::string logFile);
void InitLog(std::string logFile);

void Log( const char* inFormat, ... );
void LogFile( const char* inFormat, ... );
void LogMore( const char* inFormat, ... );
void SetLevel(LogLevel level);

}

#define LOG( ... ) Logger::Log( __VA_ARGS__ );
#define LOGMORE( ... ) Logger::LogMore( __VA_ARGS__ );
#endif
