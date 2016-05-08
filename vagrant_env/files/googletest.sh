#!/bin/bash
#
# Installs gtest to /opt/gtest

gtest=/opt/gtest

# Install google test
git clone https://github.com/google/googletest.git ${gtest}

gtestlib=${gtest}/lib
gtestincludes=${gtest}

# we should export that somewhere
includedir=${gtest}/googletest/include

# add the lib directory where we are going to store our static library
mkdir -p ${gtestlib}


cd ${gtest}

# cmake the make file? I'm not sure if this is needed
cmake googletest/

cd googletest/make
make

mv gtest_main.a ${gtestlib}/gtest_main.a



## Currently at /home/vagrant/googletest/googletest/make
# running
# clang++ -I /opt/gtest/googletest/include  -pthread sample1.cc sample1_unittest.cc /opt/gtest/lib/gtest_main.a  -o your_test