cmake_minimum_required(VERSION 2.8.11)

project(MSU)



include_directories(
    D:/Project/MSU/Project/MidRange_Linux/MSU_MidRange/MSU/
    D:/Project/MSU/Project/MidRange_Linux/MSU_MidRange/MSU/

)

# Linker options
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS}  ")

# Library path
set(CMAKE_LDFLAGS "${CMAKE_LDFLAGS} -L\".\" ")

# Define the C sources
set ( C_SRCS
    /cygwin/home/MSU_MidRange/Core/MsuComplainMode.c
    /cygwin/home/MSU_MidRange/Core/MsuCRC32.c
    /cygwin/home/MSU_MidRange/Core/MsuLeech.c
    /cygwin/home/MSU_MidRange/Core/MsuPdu.c
    /cygwin/home/MSU_MidRange/Core/MsuSocket.c
    /cygwin/home/MSU_MidRange/Interface/MsuFileInterface.c
    /cygwin/home/MSU_MidRange/Interface/MsuPlatformInterface.c
    /cygwin/home/MSU_MidRange/Interface/MsuSocketInterface.c
    /cygwin/home/MSU_MidRange/MsuMain.c
)

set_source_files_properties(
    ${C_SRCS} PROPERTIES COMPILE_FLAGS 
    " -Wfatal-errors -w -ansi -W -Wall -g -O0 -Wall")

add_executable(MSU ${CXX_SRCS} ${C_SRCS})

