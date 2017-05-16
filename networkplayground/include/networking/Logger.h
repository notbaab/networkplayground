#ifndef StringUtils_h
#define StringUtils_h

#include <stdio.h>
#include <string>
namespace Logger
{

enum LogLevel
{
    ALL = 0,
    TRACE = 10,
    DEBUG = 20,
    INFO = 30,
    WARNING = 40,
    ERROR = 50,
    CRITICAL = 60,
    OFF = 70,
};

std::string GetCommandLineArg( int inIndex );

std::string Sprintf( const char* inFormat, ... );
static std::string logFile;

static LogLevel sCurrentLevel = LogLevel::ALL;

void InitLog(LogLevel level);
void InitLog(LogLevel level, std::string logFile);

void Log( LogLevel level, const char* inFormat, ... );
void LogFile( const char* msg );
void SetLevel(LogLevel level);

}

#define LOG( level, ... ) Logger::Log( level, __VA_ARGS__ );
#endif
