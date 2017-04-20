#include "networking/StringUtils.h"
#include <fstream>
#include <iostream>
#include <string>

#if !_WIN32
extern const char** __argv;
extern int __argc;
void OutputDebugString( const char* inString ) { printf( "%s", inString ); }
#endif

Logger::LogLevel sCurrentLevel = Logger::LogLevel::ALL;
std::string Logger::GetCommandLineArg( int inIndex )
{
    if ( inIndex < __argc )
    {
        return std::string( __argv[inIndex] );
    }

    return std::string();
}

void Logger::InitLog(std::string logFile)
{
    Logger::logFile = logFile;
}

void Logger::SetLevel(LogLevel level)
{
    sCurrentLevel = level;
}

std::string Logger::Sprintf( const char* inFormat, ... )
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

void Logger::Log( const char* inFormat, ... )
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
   OutputDebugString( temp );
   OutputDebugString( "\n" );
}

// void Logger::Logl( const char* inFormat, ... )
// {
//     // not thread safe...
//    static char temp[4096];

//    va_list args;
//    va_start( args, inFormat );

// #if _WIN32
//    _vsnprintf_s( temp, 4096, 4096, inFormat, args );
// #else
//    vsnprintf( temp, 4096, inFormat, args );
// #endif
//    OutputDebugString( temp );
//    OutputDebugString( "\n" );
// }

void Logger::LogMore( const char* inFormat, ... )
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
   OutputDebugString( temp );
   OutputDebugString( "\n" );
}

void Logger::LogFile( const char* inFormat, ... )
{
//    // not thread safe...
//    static char temp[4096];
//    std::ofstream myfile;
//
//    va_list args;
//    va_start( args, inFormat );
//
//    vsnprintf( temp, 4096, inFormat, args );
//
//    myfile.open( Logger::logFile, std::ofstream::app );
//    if ( myfile.std::__1::ios_base::fail() )
//    {
//        std::cout << myfile.failbit;
//    }
//
//    myfile << temp;
//    myfile.close();
}
