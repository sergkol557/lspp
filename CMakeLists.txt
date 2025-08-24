cmake_minimum_required(VERSION 3.25.0)

set (CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math")

project(Lspp
  LANGUAGES CXX
  VERSION 1.0.0
)

# Define source files explicitly for better control
set(LSPP_SOURCES
    src/main.cpp
    src/lspp.cpp
    src/ArgumentParser.cpp
    src/FileOperations.cpp
    src/DisplayFormatter.cpp
    src/IconProvider.cpp
)

add_executable(ls++ ${LSPP_SOURCES})

# Add include directory for headers
target_include_directories(ls++ PRIVATE src)

# Link required libraries
find_library(PTHREAD_LIB pthread)
if(PTHREAD_LIB)
    target_link_libraries(ls++ ${PTHREAD_LIB})
endif()

install(TARGETS ls++ DESTINATION bin)

add_custom_target("uninstall" COMMENT "Uninstall installed files")
add_custom_command(
    TARGET "uninstall"
    POST_BUILD
    COMMENT "Uninstall files with install_manifest.txt"
    COMMAND xargs rm -vf < install_manifest.txt || echo Nothing in
            install_manifest.txt to be uninstalled!
)
