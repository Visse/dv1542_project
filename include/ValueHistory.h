#pragma once

#include <stddef.h>
#include <vector>
#include <algorithm>
#include <numeric>

/// @todo come up with a better name :/
template< typename Type >
class ValueHistory {
public:
    ValueHistory( const ValueHistory& ) = default;
    ValueHistory( ValueHistory &&move ) :
        mValues(std::move(move.mValues)),
        mNextIndex(move.mNextIndex),
        mSize(move.mSize)
    {}
    
    ValueHistory& operator = ( const ValueHistory& ) = default;
    ValueHistory& operator = ( ValueHistory &&move )
    {
        mValues = std::move(move.mValues);
        mNextIndex = move.mNextIndex;
        mSize = move.mSize;
    }
    
    explicit ValueHistory( size_t size = 0 ) :
        mSize(size)
    {
        mValues.resize(size);
    }
    
    template< typename OtherType >
    ValueHistory( const std::vector<OtherType> &values, size_t nextIndex ) : 
        mValues(values.begin(),values.end()), 
        mNextIndex(nextIndex),
        mSize(values.size())
    {}
    
    
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

    template< typename OtherType > 
    ValueHistory<OtherType> convertTo() const {
        return ValueHistory<OtherType>( mValues, mNextIndex );
    }
    
private:
    std::vector<Type> mValues;
    size_t mNextIndex = 0, mSize = 0;
};
