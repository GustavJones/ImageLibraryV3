cmake_minimum_required(VERSION 3.15)
project(ImageLibrary-Application)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

file(GLOB SOURCES "*.cpp")
add_executable(ImageLibrary ${SOURCES})
target_include_directories(ImageLibrary PRIVATE ../../include/ ../../external/Wepp/include/)
target_link_libraries(ImageLibrary PRIVATE Wepp-Server)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPERCASE)
set_target_properties(ImageLibrary PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${BUILD_TYPE_UPPERCASE} ${IMAGE_LIBRARY_BINARY_RUNTIME_DIR})
add_dependencies(ImageLibrary ImageLibrary-Website)