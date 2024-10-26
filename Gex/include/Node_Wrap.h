#ifndef GEX_NODE_WRAP_H
#define GEX_NODE_WRAP_H

#include "api.h"
#include "Node.h"
#include "boost/python.hpp"

namespace  Gex
{
    namespace Python
    {
        struct GEX_API Node_Wrap: public Gex::Node,
                public boost::python::wrapper<Gex::Node>
        {
            static bool pythonRegistered;

        public:
            Node_Wrap();


            Node_Wrap(const Node base);


            std::string Type() const override;


            std::string Description() const override;


            void InitAttributes() override;


            bool Evaluate(NodeAttributeData &evalCtx,
                          GraphContext &graphCtx,
                          NodeProfiler&  profiler)
            override;


            void AttributeChanged(const AttributePtr& attribute,
                                  const AttributeChange& change) override;


            static void RegisterPythonWrapper();
        };


        class GEX_API CompoundNode_Wrap: public Gex::CompoundNode,
            public boost::python::wrapper<Gex::CompoundNode>
        {

        public:
            CompoundNode_Wrap();


            CompoundNode_Wrap(const CompoundNode& base);

        private:
            static bool registered;
        public:
            static void RegisterPythonWrapper();
        };

    }
}





#endif //GEX_NODE_WRAP_H
