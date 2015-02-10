#include "yaml-cxx/Node.h"
#include "yaml-cxx/NodePriv.h"

#include "yaml-cxx/ValueNode.h"
#include "yaml-cxx/SequenceNode.h"
#include "yaml-cxx/MappingNode.h"

#include "FileUtils.h"

namespace Yaml
{
    Node::Node() :
        mPriv(nullptr)
    {
    }

    Node::Node( const Node &copy ) :
        mPriv(copy.mPriv)
    {
        if( mPriv )
        {
            mPriv->document->acquireReference();
        }
    }

    Node::Node( Node && move ) :
        mPriv(move.mPriv)
    {
        move.mPriv = nullptr;
    }

    Node &Node::operator = ( const Node &copy )
    {
        if( this == &copy )
        {
            return *this;
        }
        if( mPriv )
        {
            mPriv->document->releaseReference();
        }

        mPriv = copy.mPriv;
        if( mPriv )
        {
            mPriv->document->acquireReference();
        }

        return *this;
    }

    Node& Node::operator = ( Node && move )
    {
        if( this == &move )
        {
            return *this;
        }
        if( mPriv )
        {
            mPriv->document->releaseReference();
        }
        mPriv = move.mPriv;
        move.mPriv = nullptr;

        return *this;
    }

    Node::~Node()
    {
        if( mPriv )
        {
            mPriv->document->releaseReference();
        }
    }

    Node::Node( NodePriv *priv ) :
        mPriv(priv)
    {
    }

    ValueNode Node::asValue() const
    {
        if( !mPriv )
        {
            return ValueNode();
        }
        if( mPriv->node->type != YAML_SCALAR_NODE )
        {
            return ValueNode();
        }
        mPriv->document->acquireReference();
        return ValueNode( mPriv );
    }

    SequenceNode Node::asSequence() const
    {
        if( !mPriv )
        {
            return SequenceNode();
        }
        if( mPriv->node->type != YAML_SEQUENCE_NODE)
        {
            return SequenceNode();
        }
        mPriv->document->acquireReference();
        return SequenceNode( mPriv );
    }

    MappingNode Node::asMapping() const
    {
        if( !mPriv )
        {
            return MappingNode();
        }
        if( mPriv->node->type != YAML_MAPPING_NODE )
        {
            return MappingNode();
        }
        mPriv->document->acquireReference();
        return MappingNode( mPriv );
    }

    void nodeToString( std::ostringstream &stream, const Node &node );

    std::string Node::toString() const
    {
        if( getType() == Yaml::NT_Value )
        {
            return asValue().getValue();
        }
        std::ostringstream stream;
        nodeToString( stream, *this );
        return stream.str();
    }

    std::string Node::getTag() const
    {
        if( !mPriv )
        {
            return "";
        }
        return std::string( reinterpret_cast<const char*>(mPriv->node->tag) );
    }


    Node Node::LoadFromFile( const std::string& filename )
    {
        yaml_parser_t parser;
        yaml_parser_initialize( &parser );

        std::string filesource = FileUtils::getFileContent( filename );

        yaml_parser_set_input_string( &parser, reinterpret_cast<const yaml_char_t*>(filesource.c_str()), filesource.size() );

        yaml_document_t *document = new yaml_document_t;

        if( yaml_parser_load(&parser, document) != 1 )
        {
            throw std::runtime_error( StringUtils::strjoin("Failed to parse yaml document, error: ", parser.problem) );
        }
        yaml_parser_delete( &parser );

        return Document::LoadFromYaml(document);
    }

    NodeType Node::getType() const
    {
        if( !mPriv )
        {
            return NT_Invalid;
        }

        switch( mPriv->node->type )
        {
        case( YAML_NO_NODE ):
            return NT_Invalid;
        case( YAML_SCALAR_NODE ):
            return NT_Value;
        case( YAML_SEQUENCE_NODE ):
            return NT_Sequence;
        case( YAML_MAPPING_NODE ):
            return NT_Mapping;
        }
        return NT_Invalid;
    }

    void nodeToString( std::ostringstream& stream, const Node& node )
    {
        switch( node.getType() )
        {
        case( NT_Value ):
            stream << "\"" << node.asValue().getValue() << "\"";
            break;
        case( NT_Sequence ):
        {
            stream << "[";
            SequenceNode sequence = node.asSequence();
            size_t count = sequence.getCount();
            if( count > 0 )
            {
                nodeToString( stream, sequence.getValue(0) );
                for( size_t i = 1; i < count; ++i )
                {
                    stream << ", ";
                    nodeToString( stream, sequence.getValue(i) );
                }
            }
            stream << "]";
        }
        break;
        case( NT_Mapping ):
        {
            stream << "{";
            MappingNode mapping = node.asMapping();
            size_t count = mapping.getCount();
            if( count > 0 )
            {
                auto entry = mapping.getValue( 0 );
                nodeToString( stream, entry.first );
                stream << ": ";
                nodeToString( stream, entry.second );
                for( size_t i = 1; i < count; ++i )
                {
                    auto entry = mapping.getValue( i );
                    stream << ", ";
                    nodeToString( stream, entry.first );
                    stream << ": ";
                    nodeToString( stream, entry.second );
                }
            }
            stream << "}";
        }
        break;
        case( NT_Invalid ):
            break;
        }
    }










}
