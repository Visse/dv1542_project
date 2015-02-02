#pragma once


#include <map>
#include <string>

enum OptionType {
    OT_Float,
    OT_Int
};


typedef std::map<std::string,OptionType> OptionQuaryResult;

class OptionQuary {
public:
    virtual OptionQuaryResult quaryOptions() = 0;
    
    virtual void setOption( const std::string &option ) = 0;
    virtual void getOption( const )
    
};