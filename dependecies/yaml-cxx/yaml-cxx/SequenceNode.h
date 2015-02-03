#ifndef YAML_SEQUENCENODE_H
#define YAML_SEQUENCENODE_H

#include "yaml-cxx/Node.h"

namespace Yaml
{
    class SequenceNode :
        private Node
    {
        friend class Node;

    public:
        using Node::Node;

        size_t getCount() const;
        Node getValue ( size_t index ) const;
    };
}


#endif
