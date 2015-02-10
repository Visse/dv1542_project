#ifndef YAML_NODEPRIV_H
#define YAML_NODEPRIV_H

#include <yaml.h>

#include <vector>

namespace Yaml
{
    class Node;

    class Document;
    struct NodePriv {
        Document *document;
        yaml_node_t *node;
    };

    class Document
    {
    public:
        // takes ownership over document, document should be allcolated with new
        static Node LoadFromYaml ( yaml_document_t *document );
        Node getNode ( size_t index );

        void acquireReference();
        void releaseReference();

    private:
        Document() = default;
        ~Document();

        Document ( const Document& ) = delete;
        Document& operator = ( const Document& ) = delete;

    private:
        yaml_document_t *mDocument;
        std::vector<NodePriv> mNodes;
        int mRefCount;
    };
}


#endif
