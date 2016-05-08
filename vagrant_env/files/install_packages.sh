#!/bin/bash

yum -y upgrade

yum install -y wget \
               git \
               cmake \
               gcc-c++ \
               clang

# gotta have that fish
wget http://download.opensuse.org/repositories/shells:fish:release:2/CentOS_7/shells:fish:release:2.repo -P /etc/yum.repos.d/

yum install -y fish
