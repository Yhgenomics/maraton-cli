#!/bin/sh
rm -rf cmakebuild/
CMaker project:maraton-cli head:*.h,*.hpp src:*.cpp,*.hpp,*.cc,*.c flag:-Wall-std=c++11-pthread-DCURL_STATICLIB lib:lib/libuv.a,lib/maraton-framework.a,lib/libcurl.a
cd cmakebuild
cmake .
make -j 3
cp maraton-cli ../
cd ../
chmod 777 maraton-cli
