cmake_minimum_required(VERSION 3.16)

project(Comms)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(HEADERS
    Fudamentals/FDInstance.h
    Fudamentals/NetworkAddrRep.h
    Fudamentals/Socket.h
)

set(SOURCES
    Fudamentals/Socket.cpp
    Fudamentals/NetworkAddrRep.cpp
)


add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# target_link_libraries(${PROJECT_NAME} PRIVATE -lpthread)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/Fudamentals
)







