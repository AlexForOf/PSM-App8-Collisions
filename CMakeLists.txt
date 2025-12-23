cmake_minimum_required(VERSION 3.14)

project(Collisions)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(DCMAKE_BUILD_TYPE=Release)

set(BUILD_SHARED_LIBS OFF)

set(CMAKE_EXE_LINKER_FLAGS "-static")

include(FetchContent)
FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.x  
)
FetchContent_MakeAvailable(sfml)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(Collisions ${SOURCES})

target_include_directories(Collisions PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/particles
)

target_link_libraries(Collisions PRIVATE sfml-graphics sfml-window sfml-system)