#pragma once

#include <istream>
#include <ostream>

#include <glm/fwd.hpp>

std::ostream& operator << ( std::ostream &out, const glm::vec2 &vec );
std::ostream& operator << ( std::ostream &out, const glm::vec3 &vec );
std::ostream& operator << ( std::ostream &out, const glm::vec4 &vec );

std::istream& operator >> ( std::istream &in, glm::vec2 &vec );
std::istream& operator >> ( std::istream &in, glm::vec3 &vec );
std::istream& operator >> ( std::istream &in, glm::vec4 &vec );

std::ostream& operator << ( std::ostream &out, const glm::ivec2 &vec );
std::ostream& operator << ( std::ostream &out, const glm::ivec3 &vec );
std::ostream& operator << ( std::ostream &out, const glm::ivec4 &vec );

std::istream& operator >> ( std::istream &in, glm::ivec2 &vec );
std::istream& operator >> ( std::istream &in, glm::ivec3 &vec );
std::istream& operator >> ( std::istream &in, glm::ivec4 &vec );
