# CMakeLists.txt --- CMake project settings
#    ex) cmake -G "Visual Studio 9 2008" .
#    ex) cmake -DCMAKE_BUILD_TYPE=Release -G "MSYS Makefiles" .
##############################################################################

# CMake minimum version
cmake_minimum_required(VERSION 3.5)

# enable testing
enable_testing()

# project name and language
project(EAT CXX)

##############################################################################

# eat-test.exe
add_executable(eat-test eat-test.cpp)

# eat-test
add_test(NAME eat-test COMMAND $<TARGET_FILE:eat-test>)

##############################################################################
