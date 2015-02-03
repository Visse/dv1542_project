

file(GLOB YAML_CXX_FILES "yaml-cxx/*.h" "yaml-cxx/*.cpp")


add_library(yaml-cxx STATIC ${YAML_CXX_FILES})
target_include_directories(yaml-cxx PUBLIC ${PROJECT_DEPENDENCY_DIR}/include PRIVATE ./ ${PROJECT_INCLUDE_DIR})
target_link_libraries(yaml-cxx yaml)

install(TARGETS yaml-cxx DESTINATION ${PROJECT_DEPENDENCY_DIR}/lib)
install(FILES yaml-cxx/Node.h yaml-cxx/MappingNode.h yaml-cxx/SequenceNode.h yaml-cxx/ValueNode.h yaml-cxx/YamlCxx.h DESTINATION ${PROJECT_DEPENDENCY_DIR}/include/yaml-cxx)

