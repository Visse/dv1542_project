#include "yaml-cxx/MappingNode.h"
#include "yaml-cxx/ValueNode.h"
#include "yaml-cxx/NodePriv.h"

namespace Yaml
{
    size_t MappingNode::getCount() const
    {
        if( !mPriv )
        {
            return 0;
        }

        auto pairs = mPriv->node->data.mapping.pairs;

        return pairs.top - pairs.start;
    }

    std::pair<Node, Node> MappingNode::getValue(size_t index) const
    {
        if( !mPriv )
        {
            return std::pair<Node, Node>();
        }
        if( index >= getCount() )
        {
            return std::pair<Node, Node>();
        }

        auto pairs = mPriv->node->data.mapping.pairs;

        auto loc = pairs.start[index];
        return std::make_pair( mPriv->document->getNode(loc.key), mPriv->document->getNode(loc.value) );
    }
    
    std::vector<Node> MappingNode::getValues( const std::string &name, bool caseSensitive ) const
    {
        std::vector<Node> result;
        if( !mPriv ) {
            return result;
        }
        
        auto pairs = mPriv->node->data.mapping.pairs;
        for( size_t i=0, count = getCount(); i < count; ++i ) {
            ValueNode value = mPriv->document->getNode(pairs.start[i].key).asValue();
            
            if( caseSensitive ) {
                if( value.getValue() == name ) {
                    result.push_back( mPriv->document->getNode(pairs.start[i].value) );
                }
            }
            else {
                if( StringUtils::equalCaseInsensitive(value.getValue(), name) ) {
                    result.push_back( mPriv->document->getNode(pairs.start[i].value) );
                }
            }
        }
        
        return result;
    }
    
    Node MappingNode::getFirstValue( const std::string &name, bool caseSensitive ) const
    {
        if( !mPriv ) {
            return Node();
        }
        auto pairs = mPriv->node->data.mapping.pairs;
        for( size_t i=0, count = getCount(); i < count; ++i ) {
            ValueNode value = mPriv->document->getNode(pairs.start[i].key).asValue();
            
            if( caseSensitive ) {
                if( value.getValue() == name ) {
                    return mPriv->document->getNode( pairs.start[i].value );
                }
            }
            else {
                if( StringUtils::equalCaseInsensitive(value.getValue(), name) ) {
                    return mPriv->document->getNode( pairs.start[i].value );
                }
            }
        }
        return Node();
    }
    
}
