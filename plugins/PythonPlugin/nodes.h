#ifndef GEX_PYTHON_NODES_H
#define GEX_PYTHON_NODES_H

#include "nodes.h"
#include "Gex/include/Gex.h"


namespace PythonPlugin::Nodes
{
//    class PythonNode: public Gex::Node
//    {
//    public:
//        void InitAttributes() override;
//
//        bool Evaluate(Gex::NodeAttributeData &context,
//                      Gex::GraphContext &graphContext,
//                      Gex::NodeProfiler &profiler)
//                      override;
//    };
//
//    GENERATE_DEFAULT_BUILDER(PythonNodeBuilder, PythonNode)

    class Expression: public Gex::Node
    {
    public:
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(ExpressionBuilder, Expression)
}

#endif //GEX_PYTHON_NODES_H
