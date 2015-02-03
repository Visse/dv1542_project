#include "yaml-cxx/SequenceNode.h"
#include "yaml-cxx/NodePriv.h"

namespace Yaml
{
    size_t SequenceNode::getCount() const
    {
        if( !mPriv )
        {
            return 0;
        }

        const auto &items = mPriv->node->data.sequence.items;

        return items.top - items.start;
    }

    Node SequenceNode::getValue( size_t index ) const
    {
        if( !mPriv )
        {
            return Node();
        }
        if( index >= getCount() )
        {
            return Node();
        }

        const auto &items = mPriv->node->data.sequence.items;
        int itemIndex = items.start[index];

        return mPriv->document->getNode( itemIndex );
    }


}
