#include "yaml-cxx/ValueNode.h"
#include "yaml-cxx/NodePriv.h"

namespace Yaml
{
    const char* ValueNode::getValuePtr( size_t &lenght ) const
    {
        if( !mPriv )
        {
            lenght = 0;
            return nullptr;
        }

        lenght = mPriv->node->data.scalar.length;
        return reinterpret_cast<char*>( mPriv->node->data.scalar.value );
    }
}
