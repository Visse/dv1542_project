cmake_minimum_required(VERSION 2.6)
project(dv1542_project)


get_filename_component( PROJECT_DEPENDENCY_DIR dependecies/ ABSOLUTE )
get_filename_component( PROJECT_INCLUDE_DIR include/ ABSOLUTE )

if(UNIX OR MINGW)
    # g++ compiler flags
    add_definitions( "-DGLEW_NO_GLU -DGLM_FORCE_RADIANS" )
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Werror -std=c++11 -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=maybe-uninitialized")
endif(UNIX OR MINGW)


if(MSVS)
endif(MSVS)

link_directories( ${PROJECT_DEPENDENCY_DIR}/lib )


add_subdirectory(include/)
add_subdirectory(src/)

find_library(SDL_LIBRARY SDL2 ${PROJECT_DEPENDENCY_DIR}/lib REQUIRED)
find_path(SDL_INCLUDE_DIR SDL2/SDL.h ${PROJECT_DEPENDENCY_DIR}/include REQUIRED)

find_package( OpenGL )

find_library( GLEW_LIBRARY glew glew32 PATHS ${PROJECT_DEPENDENCY_DIR}/lib )
find_library( FREEIMAGE_LIBRARY freeimage PATHS ${PROJECT_DEPENDENCY_DIR}/lib )
find_library( YAML_LIBRARY yaml PATHS ${PROJECT_DEPENDENCY_DIR}/lib )
add_definitions( "-DYAML_DECLARE_STATIC" )

add_executable(dv1542_project main.cpp ${PROJECT_SOURCE_FILES} ${PROJECT_HEADER_FILES} )

target_include_directories(dv1542_project PUBLIC ${PROJECT_INCLUDE_DIR} SDL_INCLUDE_DIR ${PROJECT_DEPENDENCY_DIR}/include)

target_link_libraries(dv1542_project ${SDL_LIBRARY} ${OPENGL_gl_LIBRARY} ${FREEIMAGE_LIBRARY} ${GLEW_LIBRARY} ${YAML_LIBRARY} )


install(TARGETS dv1542_project RUNTIME DESTINATION bin)
