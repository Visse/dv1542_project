#pragma once

#include <stddef.h>
#include <vector>
#include <algorithm>

/// @todo come up with a better name :/
template< typename Type >
class ValueHistory {
public:
    ValueHistory( size_t size = 0 ) :
        mSize(size)
    {
        mValues.resize(size);
    }
    
    void setSize( size_t size ) {
        mSize = size;
        mValues.resize(size);
    }
    size_t getSize() const {
        return mSize;
    }
    size_t getIndex() const {
        return mNextIndex;
    }
    
    const Type& getValue( size_t index ) const {
        index = (mNextIndex + index) % mSize; 
        return mValues.at(index);
    }
    
    void pushValue( const Type &value ) {
        mValues.at(mNextIndex++) = value;
        mNextIndex %= mSize;
    }
    const Type *getPtr() const {
        return mValues.data();
    }
    
    Type getAvarage() const {
        Type accumulated = std::accumulate( mValues.begin(), mValues.end(), Type() );
        return accumulated / Type(mValues.size());
    }
    
    Type getMin() const {
        if( mValues.empty() ) return Type();
        return *std::min_element( mValues.begin(), mValues.end() );
    }
    Type getMax() const {
        if( mValues.empty() ) return Type();
        return *std::max_element( mValues.begin(), mValues.end() );
    }

    
private:
    std::vector<Type> mValues;
    size_t mNextIndex = 0, mSize = 0;
};