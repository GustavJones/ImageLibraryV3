cmake_minimum_required(VERSION 3.15)
project(ImageLibrary-Importer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

file(GLOB SOURCES "*.cpp")
add_executable(ImageLibraryImporter ${SOURCES})
target_include_directories(ImageLibraryImporter PRIVATE ../../include/ ../../external/Wepp/external/GParsing/include)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPERCASE)
set_target_properties(ImageLibraryImporter PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${BUILD_TYPE_UPPERCASE} ${IMAGE_LIBRARY_BINARY_RUNTIME_DIR})