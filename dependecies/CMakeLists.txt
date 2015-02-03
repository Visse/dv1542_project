
add_subdirectory(yaml-cxx)

add_library(imgui STATIC imgui/imgui.cpp)


install(TARGETS imgui DESTINATION ${PROJECT_DEPENDENCY_DIR}/lib)

install(FILES imgui/imconfig.h imgui/imgui.h DESTINATION ${PROJECT_DEPENDENCY_DIR}/include)


