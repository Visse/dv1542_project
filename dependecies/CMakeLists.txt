
add_subdirectory(yaml-cxx)

add_library(imgui STATIC imgui.cpp)


install(TARGETS imgui DESTINATION ${PROJECT_DEPENDENCY_DIR}/lib)

install(FILES imconfig.h imgui.h DESTINATION ${PROJECT_DEPENDENCY_DIR}/include)


