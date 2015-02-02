#pragma once


#include <memory>

template< typename Type >
using SharedPtr = std::shared_ptr<Type>;

template< typename Type, typename ...Args>
SharedPtr<Type> makeSharedPtr( Args&&... args )
{
    return std::make_shared<Type>(std::forward<Args>(args)... );
}