#ifndef GEX_INPUT_REL_NODE_VIEWER_H
#define GEX_INPUT_REL_NODE_VIEWER_H

#include "ui/include/NodeViewer.h"
#include "FuncViewer.h"


namespace Gex::InputRel
{
    class Plugin_API FuncNodeViewer: public Gex::Ui::NodeViewer
    {
        FuncViewWidget* funcView;

    public:
        void Setup() override;

        void OnNodeUpdated(Gex::NodeWkPtr node) override;

        void OnAttributeUpdated(
                const AttributePtr& attribute,
                const AttributeChange& change)
                override;
    };

    GENERATE_DEFAULT_VIEWER_CREATOR(FuncNodeViewerCreator, FuncNodeViewer)
}

#endif //GEX_INPUT_REL_NODE_VIEWER_H
