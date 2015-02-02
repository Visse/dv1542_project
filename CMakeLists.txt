cmake_minimum_required(VERSION 2.6)
project(dv1542_project)


get_filename_component( PROJECT_DEPENDENCY_DIR dependecies/ ABSOLUTE )
get_filename_component( PROJECT_INCLUDE_DIR include/ ABSOLUTE )

add_definitions( "-DGLEW_NO_GLU -DGLM_FORCE_RADIANS" )
if(CMAKE_COMPILER_IS_GNUCXX)
    # g++ compiler flags
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Werror -std=c++11 -Wno-error=unused-but-set-variable -Wno-error=unused-variable")
endif(CMAKE_COMPILER_IS_GNUCXX)


if(MSVS)
endif(MSVS)

link_directories( ${PROJECT_DEPENDENCY_DIR}/lib )


add_subdirectory(include/)
add_subdirectory(src/)
add_subdirectory(dependecies/)

find_library(SDL_LIBRARY SDL2 ${PROJECT_DEPENDENCY_DIR}/lib REQUIRED)
find_path(SDL_INCLUDE_DIR SDL2/SDL.h ${PROJECT_DEPENDENCY_DIR}/include REQUIRED)

find_package( OpenGL )

find_library( GLEW_LIBRARY GLEW )
find_library( FREEIMAGEPLUS_LIBRARY freeimageplus )


add_executable(dv1542_project main.cpp ${PROJECT_SOURCE_FILES} ${PROJECT_HEADER_FILES} )

target_include_directories(dv1542_project PUBLIC ${PROJECT_INCLUDE_DIR} SDL_INCLUDE_DIR ${PROJECT_DEPENDENCY_DIR}/include)

target_link_libraries(dv1542_project ${SDL_LIBRARY} yaml-cxx imgui boost_system boost_filesystem ${OPENGL_gl_LIBRARY} ${FREEIMAGEPLUS_LIBRARY} ${GLEW_LIBRARY} )


install(TARGETS dv1542_project RUNTIME DESTINATION bin)