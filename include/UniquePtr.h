#pragma once

#include <memory>

template< typename Type >
using UniquePtr = std::unique_ptr<Type>;

template< typename Type, typename ...Args>
UniquePtr<Type> makeUniquePtr( Args&&... args )
{
    return UniquePtr<Type>( new Type(std::forward<Args>(args)...) );
}