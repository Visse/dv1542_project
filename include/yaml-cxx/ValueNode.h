#ifndef YAML_VALUENODE_H
#define YAML_VALUENODE_H

#include "yaml-cxx/Node.h"

#include "StringUtils.h"

namespace Yaml
{
    class ValueNode :
        private Node
    {
        friend class Node;
    public:
        ValueNode() = default;
        ValueNode( NodePriv *priv ) :
            Node(priv)
        {}

        template< typename Type >
        Type getValue ( const Type &defaultValue = Type(), bool *succes = nullptr ) const {
            size_t lenght;
            const char *value = getValuePtr ( lenght );
            if ( !value ) {
                if ( succes ) {
                    *succes = false;
                }
                return defaultValue;
            }
            return StringUtils::fromString<Type> ( value, lenght, defaultValue, succes );
        }

        template< typename Type >
        Type getValue( bool *succes ) const {
            return getValue<Type>( Type(), succes );
        }
        
        std::string getValue() const {
            size_t lenght;
            const char *value = getValuePtr ( lenght );

            return std::string ( value, lenght );
        }

    private:
        const char* getValuePtr ( size_t &lenght ) const;
    };
}

#endif
