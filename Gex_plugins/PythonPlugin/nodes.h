#ifndef GEX_PYTHON_NODES_H
#define GEX_PYTHON_NODES_H

#include "nodes.h"
#include "Gex/include/Gex/Gex.h"


namespace GexPython::Nodes
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


    class InlineExpression: public Gex::Node
    {
    public:
        void InitAttributes() override;

        bool Evaluate(Gex::NodeAttributeData &context,
                      Gex::GraphContext &graphContext,
                      Gex::NodeProfiler &profiler)
        override;
    };

    GENERATE_DEFAULT_BUILDER(InlineExpressionBuilder, InlineExpression)
}

#endif //GEX_PYTHON_NODES_H
