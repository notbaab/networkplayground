//
//  main.cpp
//  unit_tests
//
//  Created by Erik Parreira on 2/22/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.

#include "gtest/gtest.h"
#include <stdio.h>

// assuming on mac running test through xcode, which means we need to manually
// start the tests with RUN_ALL_TESTS. One day I learn static libraries vs
// frameworks
#ifdef __APPLE__
int main( int argc, char* argv[] )
{
    ::testing::InitGoogleTest( &argc, argv );
    std::cout << "Starting Tests" << std::endl;
    int testSuccessful = RUN_ALL_TESTS();
    std::cout << "Finished" << std::endl;
    return testSuccessful;
}
#endif
