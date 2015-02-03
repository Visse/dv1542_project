#ifndef MAPPINGNODE_H
#define MAPPINGNODE_H

#include "yaml-cxx/Node.h"
#include <vector>

namespace Yaml
{
    class MappingNode :
        private Node
    {
        friend class Node;

    public:
        using Node::Node;

        size_t getCount() const;
        std::pair<Node,Node> getValue ( size_t index ) const;
        std::vector<Node> getValues( const std::string &name, bool caseSensitive = true ) const;
        Node getFirstValue( const std::string &name, bool caseSensitive = true ) const;
    };
}


#endif
