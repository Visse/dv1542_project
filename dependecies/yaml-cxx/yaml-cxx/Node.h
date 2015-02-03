#ifndef YAML_NODE_H
#define YAML_NODE_H

#include <string>

namespace Yaml
{
    struct NodePriv;

    class ValueNode;
    class SequenceNode;
    class MappingNode;
    class Document;

    enum NodeType {
        NT_Invalid,
        NT_Value,
        NT_Sequence,
        NT_Mapping
    };

    class Node
    {
        friend class ValueNode;
        friend class SequenceNode;
        friend class MappingNode;
        friend class Document;

    public:
        static Node LoadFromFile ( const std::string &filename );

    public:
        Node();
        Node ( const Node &copy );
        Node ( Node &&move );

        ~Node();

        Node& operator = ( const Node &copy );
        Node& operator = ( Node &&move );

        NodeType getType() const;

        ValueNode asValue() const;
        SequenceNode asSequence() const;
        MappingNode asMapping() const;

        std::string toString() const;

        std::string getTag() const;

        operator bool () const {
            return mPriv != nullptr;
        }
    private:
        Node ( NodePriv *priv );

        void toStringQoute() const;

    private:
        NodePriv *mPriv;
    };

}


#endif
