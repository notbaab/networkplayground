//
//  main.cpp
//  networkplaygroundclient
//
//  Created by Erik Parreira on 2/15/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include "networking/networking.h"

int main(int argc, const char * argv[]) {
    // Connect to server
    std::cout << "Hello, World!\n";
    NetworkManager nm = NetworkManager();
    nm.Init(3200);
    while (true) {
        nm.ReadIncomingPacketsIntoQueue();
        std::cout << "blah" << std::endl;
        sleep(1);
    }
    return 0;
}
