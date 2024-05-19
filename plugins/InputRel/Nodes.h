#ifndef GEX_NODES_H
#define GEX_NODES_H

#include "Gex/include/Gex.h"

namespace Gex::InputRel
{
    class LinearRel: public Gex::Node
    {
    public:
        std::string Description() const;

        void InitAttributes() override;

        bool Evaluate(NodeAttributeData &context,
                      GraphContext &graphContext,
                      NodeProfiler &profiler) override;
    };

    GENERATE_DEFAULT_BUILDER(LinearRelBuilder, LinearRel)

}


#endif //GEX_NODES_H
