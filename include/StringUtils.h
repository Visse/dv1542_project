#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <list>

namespace StringUtils
{
    std::string stripSpaces( const std::string &str );
    std::string stripSpaces( const char *str, size_t lenght=0 );
    
    // checks if s1 starts with s2
    bool startsWith( const std::string &s1, const std::string &s2 );
    // checks if s1 starts with s2, if either l1 or l2 is 0, we auto detect the size
    bool startsWith( const char *s1, size_t l1, const char *s2, size_t l2 );
    
    
    // compares if s1 or s2 comes first, ignoring case
    bool compareCaseInsensitive( const std::string &s1, const std::string &s2 );
    // compares if s1 or s2 comes first, ignoring case, if either l1 or l2 is 0, we auto detect the size
    bool compareCaseInsensitive( const char *s1, size_t l1, const char *s2, size_t l2 );
    
    //  if s1 or s2 is equal, ignoring case
    bool equalCaseInsensitive( const std::string &s1, const std::string &s2 );
    // if s1 or s2 is equal, ignoring case, if either l1 or l2 is 0, we auto detect the size
    bool equalCaseInsensitive( const char *s1, size_t l1, const char *s2, size_t l2 );
    
    // splits the string in two, removing the token in the procces
    std::pair<std::string, std::string> splitInTwo( const std::string &s1, const std::string &s2 );
    // splits the string in two, removing the token in the procces, if either l1 or l2 is 0, we auto detect the size
    std::pair<std::string, std::string> splitInTwo( const char *s1, size_t l1, const char *s2, size_t l2 );
    
    std::string toLowerCase( const std::string &str );
    std::string toLowerCase( const char *str, size_t lenght=0 );
    
    std::string toUpperCase( const std::string &str );
    std::string toUpperCase( const char *str, size_t lenght=0 );
    
    void toLowerCaseInPlace( std::string &str );
    void toLowerCaseInPlace( char *str, size_t lenght=0 );
    
    void toUpperCaseInPlace( std::string &str );
    void toUpperCaseInPlace( char *str, size_t lenght=0 );
    
    // test the string for a substrign described by pattern,
    // the test is ignoring case, and the special token '*' is a wild card
    bool testString( const std::string &str, const std::string &pattern );
    bool testString( const char *str, size_t lenght, const char *pattern, size_t patternLenght );
    
    struct CaseInsensitiveCompare {
        bool operator () ( const std::string &s1, const std::string &s2 ) const {
            return compareCaseInsensitive( s1, s2 );
        }
    };

    struct CaseInsensitiveEqual {
        bool operator () ( const std::string &s1, const std::string &s2 ) const {
            return equalCaseInsensitive( s1, s2 );
        }
    };

    template< typename... Args >
    std::string strjoin( const Args&... args );

    template< typename... Args >
    int scanf( std::istream &input, const std::string &format, Args&... args );
    
    template< typename... Args >
    int sscanf( const std::string &input, const std::string &format, Args&... args );

    template< typename Type >
    Type fromString( const std::string &str, const Type& defaultValue = Type(), bool *succes=nullptr );
    
    template< typename Type >
    Type fromString( const char *str, size_t lenght, const Type& defaultValue = Type(), bool *succes=nullptr );

    template< typename Type >
    std::string toString( const Type &type );

    template< typename Container = std::list<std::string>, typename TokenContainer = std::list<std::string> >
    Container split( const std::string &str, const TokenContainer &tokens, bool keepTokens = false, bool keepEmpty = false );
    
    template< typename Container = std::list<std::string>, typename TokenContainer = std::list<std::string> >
    Container split( const char *str, size_t lenght, const TokenContainer &tokens, bool keepTokens = false, bool keepEmpty = false );
    
    template< typename... Args >
    std::string format ( const std::string &format, const Args&... args );
}

#ifndef STRINGUTILS_NO_IMPLEMENTATION
#   include "StringUtils.cpp"
#endif

#endif
