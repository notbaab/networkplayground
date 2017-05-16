#include "networking/Logger.h"
#include <fstream>
#include <iostream>

#if !_WIN32
extern const char** __argv;
extern int __argc;
void OutputDebugString( const char* inString ) { printf( "%s", inString ); }
#endif

namespace Logger
{


bool logToStdOut = false;

// TODO: Move somewhere sane
std::string GetCommandLineArg( int inIndex )
{
    if ( inIndex < __argc )
    {
        return std::string( __argv[inIndex] );
    }

    return std::string();
}

void InitLog(LogLevel level, std::string logFile)
{
    logFile = logFile;
    SetLevel(level);
}

void InitLog(LogLevel level)
{
    logToStdOut = true;
    SetLevel(level);
}

void SetLevel(LogLevel level)
{
    sCurrentLevel = level;
}

std::string Sprintf( const char* inFormat, ... )
{
    // not thread safe...
    static char temp[4096];

    va_list args;
    va_start( args, inFormat );

#if _WIN32
    _vsnprintf_s( temp, 4096, 4096, inFormat, args );
#else
    vsnprintf( temp, 4096, inFormat, args );
#endif
    return std::string( temp );
}

void Log( LogLevel level, const char* inFormat, ... )
{
    if ( level < sCurrentLevel )
    {
        return;
    }

    // not thread safe...
   static char temp[4096];

   va_list args;
   va_start( args, inFormat );

#if _WIN32
   _vsnprintf_s( temp, 4096, 4096, inFormat, args );
#else
   vsnprintf( temp, 4096, inFormat, args );
#endif
    if (logToStdOut)
    {
        OutputDebugString( temp );
        OutputDebugString( "\n" );
    } else {
        LogFile(temp);
    }
}

// TOOD: Broken
void LogFile( const char* msg )
{
    std::cout <<"IN HERE";
   std::ofstream myfile;

   myfile.open( logFile, std::ofstream::app );
   if ( myfile.std::__1::ios_base::fail() )
   {
       std::cout << myfile.failbit;
   }

   myfile << msg;
   myfile.close();
}

}