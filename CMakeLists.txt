cmake_minimum_required(VERSION 3.13)
project(TP1_cli_srv C)

set(CMAKE_C_STANDARD 99)

##NEW --------------------------------------------------------
# Add main.cpp file of project root directory as source file
#set(SOURCE_FILES ./src/main.c)
##------------------------------------------------------------


add_executable(TP1_cli_srv src/server/main.c)
##add_executable(TP1_cli_srv ${SOURCE_FILES})