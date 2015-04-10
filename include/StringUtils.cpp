#include "StringUtils.h"

#include <string>
#include <cstring>
#include <utility>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <cctype>

namespace StringUtils
{
    namespace internal
    {
        inline std::string stripSpaces( const char *str, size_t length )
        {
            if( length == 0 ) return std::string();
            
            size_t start = 0, end = length - 1;
            // strip all spaces from the beginning
            for( ; start < end; start++ ) {
                if( !std::isspace(str[start]) ) {
                    break;
                }
            }
            // strip all spaces from the end
            for( ; start < end; end-- ) {
                if( !std::isspace(str[end]) ) {
                    end++;
                    break;
                }
            }
            return std::string( str+start, end-start );
        }
        
        inline bool startsWith( const char *s1, size_t l1, const char *s2, size_t l2 )
        {
            if( l1 < l2 ) return false;
            
            for( size_t i=0; i < l2; ++i ) {
                if( s1[i] != s2[i] ) {
                    return false;
                }
            }
            return true;
        }
        
        inline bool endsWith( const char *s1, size_t l1, const char *s2, size_t l2 )
        {
            if( l1 < l2 ) return false;
            
            for( size_t i=l1-l2; i < l1; ++i ) {
                if( s1[i] != s2[i-l2] ) {
                    return false;
                }
            }
            return true;
        }
        
        inline bool caseInsensitiveCompare( const char *s1, size_t l1, const char *s2, size_t l2 ) 
        {
            if( l1 < l2 ) return true;
            if( l1 > l2 ) return false;
            
            for( size_t i=0; i < l1; ++i ) {
                char c1 = std::tolower( s1[i] ),
                     c2 = std::tolower( s2[i] );

                if( c1 < c2 ) {
                    return true;
                }
                if( c1 > c2 ) {
                    return false;
                }
            }
            return false;
        }
        
        inline bool caseInsensitiveEqual( const char *s1, size_t l1, const char *s2, size_t l2 )
        {
            if( l1 != l2 ) return false;
            
            for( size_t i=0; i < l1; ++i ) {
                char c1 = std::tolower( s1[i] ),
                     c2 = std::tolower( s2[i] );
                     
                if( c1 != c2 ) {
                    return false;
                }
            }
            return true;
        }
    
        inline std::pair<std::string,std::string> splitInTwo( const char *s1, size_t l1, 
                                                              const char *s2, size_t l2 )
        {
            const char *e1 = s1 + l1,
                       *e2 = s2 + l2;
            const char *split = std::search( s1, e1, s2, e2 );
            
            if( split == e2 ) {
                // token was not found
                return std::make_pair( std::string(s1,l1), std::string() );
            }
            
            size_t pos = std::distance(s1,split);
            
            return std::make_pair( std::string(s1,pos), std::string(split+l2,l1-l2-pos) );
        }
    
        inline void toLowerCase( char *str, size_t lenght )
        {
            std::transform(
                str, str+lenght, str, (int(*)(int))std::tolower
            );
        }
        
        inline void toUpperCase( char *str, size_t lenght )
        {
            std::transform(
                str, str+lenght, str, (int(*)(int))std::toupper
            );
        }
    }

    inline std::string stripSpaces( const std::string &str )
    {
        return internal::stripSpaces( str.c_str(), str.size() );
    }
    
    inline std::string stripSpaces( const char *str, size_t lenght )
    {
        if( lenght == 0 ) {
            lenght = std::strlen( str );
        }
        return internal::stripSpaces( str, lenght );
    }

    inline bool startsWith( const std::string &s1, const std::string &s2 )
    {
        return internal::startsWith( s1.c_str(), s1.size(), s2.c_str(), s2.size() );
    }

    inline bool startsWith( const char *s1, size_t l1, const char *s2, size_t l2 )
    {
        if( l1 == 0 ) {
            l1 = std::strlen( s1 );
        }
        if( l2 == 0 ) {
            l2 = std::strlen( s2 );
        }
        return internal::startsWith( s1, l1, s2, l2 );
    }

    inline bool compareCaseInsensitive( const std::string &s1, const std::string &s2 )
    {
        return internal::caseInsensitiveCompare( s1.c_str(), s1.size(), s2.c_str(), s2.size() );
    }
    
    inline bool compareCaseInsensitive( const char *s1, size_t l1, const char *s2, size_t l2 )
    {
        if( l1 == 0 ) {
            l1 = std::strlen( s1 );
        }
        if( l2 == 0 ) {
            l2 = std::strlen( s2 );
        }
        return internal::caseInsensitiveCompare( s1, l1, s2, l2 );
    }
    
    inline bool equalCaseInsensitive( const std::string &s1, const std::string &s2 )
    {
        return internal::caseInsensitiveEqual( s1.c_str(), s1.size(), s2.c_str(), s2.size() );
    }
    
    inline bool equalCaseInsensitive( const char *s1, size_t l1, const char *s2, size_t l2 )
    {
        if( l1 == 0 ) {
            l1 = std::strlen( s1 );
        }
        if( l2 == 0 ) {
            l2 = std::strlen( s2 );
        }
        return internal::caseInsensitiveEqual( s1, l1, s2, l2 );
    }
    
    inline std::pair<std::string, std::string> splitInTwo( const std::string &s1, const std::string &s2 )
    {
        return internal::splitInTwo( s1.c_str(), s1.size(), s2.c_str(), s2.size() );
    }
    
    inline std::pair<std::string, std::string> splitInTwo( const char *s1, size_t l1, const char *s2, size_t l2 )
    {
        if( l1 == 0 ) {
            l1 = std::strlen( s1 );
        }
        if( l2 == 0 ) {
            l2 = std::strlen( s2 );
        }
        return internal::splitInTwo( s1, l1, s2, l2 );
    }
    
    inline std::string toLowerCase( const std::string &str )
    {
        std::string res( str );
        internal::toLowerCase( &res[0], res.size() );
        return res;
    }
    
    inline std::string toLowerCase( const char *str, size_t lenght )
    {
        if( lenght == 0 ) {
            lenght = std::strlen(str);
        }
        std::string res( str, lenght );
        internal::toLowerCase( &res[0], res.size() );
        return res;
    }
    
    inline std::string toUpperCase( const std::string &str )
    {
        std::string res( str );
        internal::toUpperCase( &res[0], res.size() );
        return res;
    }
    
    inline std::string toUpperCase( const char *str, size_t lenght )
    {
        if( lenght == 0 ) {
            lenght = std::strlen(str);
        }
        std::string res( str, lenght );
        internal::toUpperCase( &res[0], res.size() );
        return res;
    }

    inline void toLowerCaseInPlace( std::string &str )
    {
        return internal::toLowerCase( &str[0], str.size() );
    }
    
    inline void toLowerCaseInPlace( char *str, size_t lenght )
    {
        if( lenght == 0 ) {
            lenght = std::strlen(str);
        }
        return internal::toLowerCase( str, lenght );
    }
    
    inline void toUpperCaseInPlace( std::string &str )
    {
        return internal::toUpperCase( &str[0], str.size() );
    }
    
    inline void toUpperCaseInPlace( char *str, size_t lenght )
    {
        if( lenght == 0 ) {
            lenght = std::strlen(str);
        }
        return internal::toUpperCase( str, lenght );
    }
    
    inline bool testString( const std::string &str, const std::string &pattern )
    {
        return testString( str.c_str(), str.size(), pattern.c_str(), pattern.size() );
    }
    
    inline bool testString( const char *str, size_t lenght, const char *pattern, size_t patternLenght )
    {
        if( lenght == 0 ) {
            lenght = std::strlen(str);
        }
        if( patternLenght == 0 ) {
            patternLenght = std::strlen(pattern);
        }
        return std::search( 
            str, str+lenght, pattern, pattern+patternLenght,
            []( char s, char p ) {
                if( p == '*' ) { 
                    return true;
                }
                return std::tolower(s) == std::tolower(p);
            }
        ) != (str + lenght);
    }
    
    namespace internal 
    {
        class InMemoryIOBuffer :
            public std::streambuf
        {
        public:
            InMemoryIOBuffer( const char *buffer, size_t size ) :
                mStart(buffer), 
                mCur(buffer), 
                mEnd(buffer+size) 
            {
                refillBuffer();
            }

            virtual int underflow() {
                if( !refillBuffer() ) {
                    return traits_type::eof();
                }
                return mBuffer[0];
            }

            bool refillBuffer() {
                if( mCur == mEnd ) {
                    return false;
                }

                size_t size = std::min<size_t>( mEnd-mCur, 32 );
                std::memcpy( mBuffer, mCur, size );
                mCur += size;

                setg( mBuffer, mBuffer, mBuffer+size );
                return true;
            }

        private:
            char mBuffer[32];
            const char *mStart,
                *mCur,
                *mEnd;
        };
           
        template< typename... Args>
        struct StrJoin;
        
        template<>
        struct StrJoin<>
        {
            static void join( std::ostringstream &stream ) {}
        };
        
        template< typename Head, typename... Args >
        struct StrJoin<Head,Args...> 
        {
            static void join( std::ostringstream &stream, const Head &head, const Args&... args ) 
            {
                stream << head;
                StrJoin<Args...>::join(stream,args...);
            }
        };
        
        template< int N, typename... Args >
        struct Scanf;

        template< int N >
        struct Scanf<N> {
            static int scanf( std::istream &input, std::string::const_iterator cur, std::string::const_iterator end ) {
                return N;
            }
        };

        template< int N, typename Head, typename... Args >
        struct Scanf<N,Head,Args...> {
            static int scanf( std::istream &input, std::string::const_iterator cur, std::string::const_iterator end, Head &head, Args&... args ) {
                for( ; cur != end; ++cur ) {
                    if( isspace(*cur) ) {
                        input >> std::ws;
                        continue;
                    } else if( *cur == '%' ) {
                        ++cur;
                        if( cur == end ) {
                            throw std::runtime_error( "Invalid format string!" );
                        }
                        if( *cur == 's' ) {
                            input >> head;
                            ++cur;
                            return Scanf<N+1,Args...>::scanf( input, cur, end, args... );
                        }
                    }
                    char c = input.get();
                    if( c != *cur ) {
                        // input mismatch
                        return N;
                    }
                }
                return N;
            }
        };
    
        template< typename Type >
        struct FromString {
            static Type fromString( const char *str, size_t lenght, const Type& defaultValue, bool *succes ) {
                internal::InMemoryIOBuffer buffer( str, lenght );
                std::istream stream( &buffer );
                Type type;
                stream >> type;
                if( !stream ) {
                    *succes = false;
                    return defaultValue;
                }
                *succes = true;
                return type;
            }
        };
        
        template<>
        struct FromString<std::string> {
            static std::string fromString( const char *str, size_t lenght, const std::string& defaultValue, bool *succes ) {
                *succes = true;
                return std::string( str, lenght );
            }
        };
        
        template<>
        struct FromString<bool> {
            static bool fromString( const char *str, size_t lenght, const bool& defaultValue, bool *succes ) {
                static const char *VALID_TRUE_STRINGS[] = {
                    "true", "1"
                };
                static const char *VALID_FALSE_STRINGS[] = {
                    "false", "0"
                };
                
                for( const char *trueStr : VALID_TRUE_STRINGS ) {
                    if( equalCaseInsensitive(trueStr, 0, str, lenght) ) {
                        *succes = true;
                        return true;
                    }
                }
                for( const char *falseStr : VALID_FALSE_STRINGS ) {
                    if( equalCaseInsensitive(falseStr, 0, str, lenght) ) {
                        *succes = true;
                        return false;
                    }
                }
                *succes = false;
                return defaultValue;
            }
        };
      
        struct HasStdToString {};
        
        template< typename Type >
        std::conditional<true,HasStdToString,decltype(std::to_string(std::declval<Type>()))>
        checkForStdToString( const Type &);
        void checkForStdToString( ... );
        
        template< typename Type >
        using CheckForStdToString = typename std::is_same<HasStdToString,decltype(checkForStdToString(std::declval<Type>()))>;
        
        template< typename Type, bool HasStdToString >
        struct ToString {
            static std::string toString( const Type &type ) {
                return std::to_string(type);
            }
        };
        
        template< typename Type >
        struct ToString<Type,false> {
            static std::string toString( const Type &type ) {
                std::ostringstream stream;
                stream << type;
                return stream.str();
            }
        };
        
        template< bool _>
        struct ToString<std::string,_> {
            static std::string toString( const std::string &type ) {
                return type;
            }
        };
    
        template< typename Type >
        struct StringSizeAndCharPtr;
        
        template<>
        struct StringSizeAndCharPtr<std::string>
        {
            static size_t getSize( const std::string &s ) {
                return s.size();
            }
            static const char* getPtr( const std::string &s ) {
                return s.c_str();
            }
        };
       
        template<>
        struct StringSizeAndCharPtr<char*>
        {
            static size_t getSize( const char *s ) {
                return std::strlen(s);
            }
            static const char* getPtr( const char *s ) {
                return s;
            }
        };
        
        
        template< typename Container, typename TokenContainer >
        Container split( const char *str, size_t lenght, const TokenContainer &tokens, bool keepTokens, bool keepEmpty )
        {
            Container result;
            typedef typename std::remove_const<
                        typename std::remove_reference<
                            decltype(
                                *std::begin(std::declval<TokenContainer>())
                            )
                        >::type
                    >::type TokenType;
            typedef StringSizeAndCharPtr<TokenType> SizeAndCharPtr;
            
            size_t start = 0;
            for( size_t i=0; i < lenght; ++i ) 
            {
                for( const auto &token : tokens ) {
                    if( startsWith(str+i, lenght-i, SizeAndCharPtr::getPtr(token), SizeAndCharPtr::getSize(token)) ) {
                        if( start != i || (start == i && keepEmpty) ) {
                            result.push_back( std::string(str+start,i-start) );
                            start = i;
                        }
                        if( keepTokens ) {
                            result.push_back( std::string(token) );
                        }
                        i += SizeAndCharPtr::getSize(token) - 1;
                        start = i+1;
                        break;
                    }
                }
            }
            
            if( start != lenght ) {
                result.push_back(std::string(str+start,lenght-start) );
            }
            return result;
        }
        
        
        template< typename... Args >
        struct Format;

        template< typename Head, typename... Args >
        struct Format<Head,Args...> {
            static void format( std::ostringstream &stream,  
                                const char *cur,  const char *end, 
                                const Head &head, const Args&... args ) 
            {
                // find our first '%'
                do {
                    const char *format = std::find( cur, end, '%' );
                    stream.write( cur, format-cur );
                    // did we find anything?
                    if( cur == end ) {
                        return;
                    }
                    cur = format;
                    ++cur;
                    
                    // the format string can't end with a single '%'
                    if( cur == end ) {
                        throw std::runtime_error( "Invalid format string!" );
                    }
                    // where there '%%'?
                    if( *cur != '%' ) {
                        break;
                    }
                    // if so it were escaped so print a single '%'
                    stream << '%';
                } while ( true );
                    
                // check our format, '%[width][.precision]s
                int width = -1;
                if( std::isdigit(*cur) ) {
                    width = 0;
                    while( cur != end && std::isdigit(*cur) ) {
                        width *= 10;
                        width += cur - '0';
                        cur++;
                    }
                    if( cur == end ) {
                        throw std::runtime_error( "Invalid format string!" );
                    }
                }
                int precision = -1;
                if( *cur == '.' ) {
                    ++cur;
                    if( cur == end || !std::isdigit(*cur) ) {
                        throw std::runtime_error( "Invalid format string!" );
                    }
                    precision = 0;
                    while( cur != end && std::isdigit(*cur) ) {
                        precision *= 10;
                        precision += cur - '0';
                        cur++;
                    }
                    if( cur == end ) {
                        throw std::runtime_error( "Invalid format string!" );
                    }
                }
                if( *cur != 's' ) {
                    throw std::runtime_error( "Invalid format string!" );
                }
                ++cur;

                if( width >= 0 ) {
                    std::ostringstream tmp;
                    if( precision >= 0 ) {
                        tmp.precision ( precision );
                        tmp.setf( std::ios::floatfield, std::ios::fixed );
                    }
                    tmp << head;
                    
                    size_t tmpWidth = tmp.tellp();
                    tmp.seekp( 0, std::ios::beg );
                    tmpWidth -= tmp.tellp();
                    
                    for( size_t i=width; i > tmpWidth; --i ) {
                        stream << ' ';
                    }
                    stream << tmp.rdbuf();
                } 
                else {
                    auto curPrecision = stream.precision();
                    auto curFlags = stream.flags();
                    if( precision >= 0 ) {
                        stream.precision( precision );
                        stream.setf( std::ios::floatfield, std::ios::fixed );
                    }
                    stream << head;
                    stream.flags( curFlags );
                    stream.precision( curPrecision );
                }

                Format<Args...>::format( stream, cur, end, args... );
            }
        };

        template<>
        struct Format<> {
            static void format( std::ostringstream &stream, const char *cur,  const char *end ) 
            {
                stream.write ( &*cur, end-cur );
            }
        };
    }
    
    template< typename... Args >
    std::string strjoin( const Args&... args )
    {
        std::ostringstream stream;

        // you could use something like:
        // 'dummy( (operator << (stream,args),0)... );'
        //    - where dummy is needed to get the right scope (a comma seperated list)
        // However this doesn't work since the order matter :(
        //   - the compiler is free to eveluate function argument in any order
        internal::StrJoin<Args...>::join( stream, args... );

        return stream.str();
    }
    
    template< typename... Args >
    int scanf( std::istream &input, const std::string &format, Args&... args )
    {
        return internal::Scanf<0,Args...>::scanf( input, format.cbegin(), format.cend(), args... );
    }

    template< typename... Args >
    int sscanf( const std::string &input, const std::string &format, Args&... args )
    {
        internal::InMemoryIOBuffer ioBuffer( input.c_str(), input.size() );
        std::istream stream( &ioBuffer );
        return internal::Scanf<0,Args...>::scanf( stream, format.cbegin(), format.cend(), args... );
    }
    
    template< typename Type >
    Type fromString( const std::string &str, const Type& defaultValue, bool *succes )
    {
        bool dummy;
        if( !succes ) {
            succes = &dummy;
        }
        return internal::FromString<Type>::fromString( str.c_str(), str.size(), defaultValue, succes );
    }
    
    template< typename Type >
    Type fromString( const char *str, size_t lenght, const Type& defaultValue, bool *succes )
    {
        bool dummy;
        if( !succes ) {
            succes = &dummy;
        }
        return internal::FromString<Type>::fromString( str, lenght, defaultValue, succes );
    }

    template< typename Type >
    std::string toString( const Type &type )
    {
        return internal::ToString<Type,internal::CheckForStdToString<Type>::value>::toString( type );
    }
        
    template< typename Container, typename TokenContainer >
    Container split( const std::string &str, const TokenContainer &tokens, bool keepTokens, bool keepEmpty )
    {
        return internal::split<Container,TokenContainer>( str.c_str(), str.size(), tokens, keepTokens, keepEmpty );
    }
       
    template< typename Container, typename TokenContainer >
    Container split( const char *str, size_t lenght, const TokenContainer &tokens, bool keepTokens, bool keepEmpty )
    {
        if( lenght == 0 ) {
            lenght = std::strlen(str);
        }
        return internal::split<Container,TokenContainer>( str, lenght, tokens, keepTokens, keepEmpty );
    }
    
    
    template< typename... Args >
    std::string format( const std::string &format, const Args&... args )
    {
        std::ostringstream stream;
        internal::Format<Args...>::format( stream, format.c_str(), format.c_str()+format.size(), args... );

        return stream.str();
    }
    
}