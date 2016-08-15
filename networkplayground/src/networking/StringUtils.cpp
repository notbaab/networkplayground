#include "networking/StringUtils.h"
#include <fstream>
#include <iostream>
#include <string>

#if !_WIN32
extern const char** __argv;
extern int __argc;
void OutputDebugString( const char* inString ) { printf( "%s", inString ); }
#endif

std::string StringUtils::GetCommandLineArg( int inIndex )
{
    if ( inIndex < __argc )
    {
        return std::string( __argv[inIndex] );
    }

    return std::string();
}

void StringUtils::initLog( std::string logFile )
{
    StringUtils::logFile = logFile;
}

std::string StringUtils::Sprintf( const char* inFormat, ... )
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

void StringUtils::Log( const char* inFormat, ... )
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

void StringUtils::LogFile( const char* inFormat, ... )
{
    // not thread safe...
    static char temp[4096];
    std::ofstream myfile;

    va_list args;
    va_start( args, inFormat );

    vsnprintf( temp, 4096, inFormat, args );

    myfile.open( StringUtils::logFile, std::ofstream::app );
    if ( myfile.std::__1::ios_base::fail() )
    {
        std::cout << myfile.failbit;
    }

    myfile << temp;
    myfile.close();
}
