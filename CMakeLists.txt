# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)
# Optionally, set C standard (e.g., C11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CPP_STANDARD 11)

# Set the project name
project(concerter)

# Add the executable and specify the C file(s) to compile
add_executable(concerter
                classes/program.cpp
                classes/event.cpp
                main.cpp
                )

add_library(sqliteConn STATIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/sqlite/sqlite_conn.c)
find_package(SQLite3 REQUIRED)

target_include_directories(concerter PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/headers)
target_include_directories(concerter PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/libs/sqlite)

target_link_libraries(concerter
                    sqliteConn
                    SQLite::SQLite3)




