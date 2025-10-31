message(STATUS "Configuring ImageLibrary-Website")

find_program(NPM_EXECUTABLE "npm" REQUIRED)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/data")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/data/scripts")

file(GLOB HTML_FILES "${CMAKE_SOURCE_DIR}/data/*.html")
file(GLOB SCRIPT_FILES "${CMAKE_SOURCE_DIR}/data/scripts/*.js")
set(STYLE_DIRECTORY "${CMAKE_SOURCE_DIR}/data/style")
set(ASSETS_DIRECTORY "${CMAKE_SOURCE_DIR}/data/assets")

add_custom_target(ImageLibrary-Website-Dependencies ALL COMMAND ${NPM_EXECUTABLE} install WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/data COMMENT "Installing npm dependencies")
add_custom_target(ImageLibrary-Website ALL COMMAND ${NPM_EXECUTABLE} run build WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/data COMMENT "Building ImageLibrary-Website" DEPENDS ImageLibrary-Website-Dependencies)

foreach(HTML_FILE ${HTML_FILES})
  add_custom_command(TARGET ImageLibrary-Website POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${HTML_FILE} ${IMAGE_LIBRARY_BINARY_RUNTIME_DIR}/data)
endforeach()

foreach(SCRIPT_FILE ${SCRIPT_FILES})
  add_custom_command(TARGET ImageLibrary-Website POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SCRIPT_FILE} ${IMAGE_LIBRARY_BINARY_RUNTIME_DIR}/data/scripts)
endforeach()

add_custom_command(TARGET ImageLibrary-Website POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${STYLE_DIRECTORY} ${IMAGE_LIBRARY_BINARY_RUNTIME_DIR}/data/style)
add_custom_command(TARGET ImageLibrary-Website POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIRECTORY} ${IMAGE_LIBRARY_BINARY_RUNTIME_DIR}/data/assets)