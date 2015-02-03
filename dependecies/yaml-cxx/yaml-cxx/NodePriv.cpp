#include "yaml-cxx/NodePriv.h"
#include "yaml-cxx/Node.h"

namespace Yaml
{
    Node Document::LoadFromYaml( yaml_document_t* document )
    {
        Document *doc = new Document;
        doc->mDocument = document;
        doc->mRefCount = 0;

        doc->mNodes.reserve( document->nodes.top - document->nodes.start );

        for( yaml_node_t *node = document->nodes.start; node != document->nodes.top; ++node )
        {
            NodePriv priv;
            priv.document = doc;
            priv.node = node;
            doc->mNodes.push_back( priv );
        }

        if( doc->mNodes.empty() )
        {
            delete doc;
            return Node();
        }

        return Node( doc->getNode(1) );
    }

    Node Document::getNode( size_t index )
    {
        if( index == 0 || index > mNodes.size() )
        {
            return Node();
        }
        acquireReference();
        return Node( &mNodes.at(index - 1) );
    }

    Document::~Document()
    {
        yaml_document_delete( mDocument );
        delete mDocument;
    }

    void Document::acquireReference()
    {
        mRefCount++;
    }

    void Document::releaseReference()
    {
        mRefCount--;
        if( mRefCount == 0 )
        {
            delete this;
        }
    }


}
