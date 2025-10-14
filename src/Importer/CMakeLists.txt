cmake_minimum_required(VERSION 3.15)
project(ImageLibrary-Importer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

file(GLOB SOURCES "*.cpp")
add_executable(ImageLibraryImporter ${SOURCES})
target_include_directories(ImageLibraryImporter PRIVATE ../../include/ ../../external/Wepp/external/GParsing/include)
set_target_properties(ImageLibraryImporter PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)