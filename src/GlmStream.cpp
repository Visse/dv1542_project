#include "GlmStream.h"
#include "StringUtils.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


std::ostream& operator << ( std::ostream &out, const glm::vec2 &vec )
{
    out << "{" << vec.x << "," << vec.y << "}";
    return out;
}

std::ostream& operator << ( std::ostream &out, const glm::vec3 &vec )
{
    out << "{" << vec.x << "," << vec.y << "," << vec.z << "}";
    return out;
}

std::ostream& operator << ( std::ostream &out, const glm::vec4 &vec )
{
    out << "{" << vec.x << "," << vec.y << "," << vec.z << "," << vec.w << "}";
    return out;
}

std::istream& operator >> ( std::istream &in, glm::vec2 &vec )
{
    if( StringUtils::scanf(in, "{%s,%s}", vec.x, vec.y) != 2 )
    {
        in.setstate( std::ios::failbit );
    }
    return in;
}

std::istream& operator >> ( std::istream &in, glm::vec3 &vec )
{
    if( StringUtils::scanf(in, "{%s,%s,%s}", vec.x, vec.y, vec.z) != 3 )
    {
        in.setstate( std::ios::failbit );
    }
    return in;
}

std::istream& operator >> ( std::istream &in, glm::vec4 &vec )
{
    if( StringUtils::scanf(in, "{%s,%s,%s,%s}", vec.x, vec.y, vec.z, vec.w) != 4 )
    {
        in.setstate( std::ios::failbit );
    }
    return in;
}

// ivec*

std::ostream& operator << ( std::ostream &out, const glm::ivec2 &vec )
{
    out << "{" << vec.x << "," << vec.y << "}";
    return out;
}

std::ostream& operator << ( std::ostream &out, const glm::ivec3 &vec )
{
    out << "{" << vec.x << "," << vec.y << "," << vec.z << "}";
    return out;
}

std::ostream& operator << ( std::ostream &out, const glm::ivec4 &vec )
{
    out << "{" << vec.x << "," << vec.y << "," << vec.z << "," << vec.w << "}";
    return out;
}

std::istream& operator >> ( std::istream &in, glm::ivec2 &vec )
{
    if( StringUtils::scanf(in, "{%s,%s}", vec.x, vec.y) != 2 )
    {
        in.setstate( std::ios::failbit );
    }
    return in;
}

std::istream& operator >> ( std::istream &in, glm::ivec3 &vec )
{
    if( StringUtils::scanf(in, "{%s,%s,%s}", vec.x, vec.y, vec.z) != 3 )
    {
        in.setstate( std::ios::failbit );
    }
    return in;
}

std::istream& operator >> ( std::istream &in, glm::ivec4 &vec )
{
    if( StringUtils::scanf(in, "{%s,%s,%s,%s}", vec.x, vec.y, vec.z, vec.w) != 4 )
    {
        in.setstate( std::ios::failbit );
    }
    return in;
}
