#ifndef GEX_NODE_WRAP_H
#define GEX_NODE_WRAP_H

#include "api.h"
#include "Node.h"
#include "pybind11/pybind11.h"

#include "Gex/python/utils.h"


WEAK_PTR_CONVERTER(Gex::NodeWkPtr)


namespace  Gex
{
    namespace Python
    {
        struct GEX_API Node_Wrap: public Gex::Node
        {
            static bool pythonRegistered;
            static PyClassRegistry registry;

        public:
            Node_Wrap();

            Node_Wrap(const Node_Wrap& other) = delete;

            ~Node_Wrap() override;

            std::string Type() const override;


            std::string Description() const override;


            void InitAttributes() override;


            bool Evaluate(NodeAttributeData &evalCtx,
                          GraphContext &graphCtx,
                          NodeProfiler&  profiler)
            override;


            void AttributeChanged(const AttributePtr& attribute,
                                  const AttributeChange& change) override;


            static bool RegisterPythonWrapper(pybind11::module_& mod,
                                              PyThreadState* state=nullptr);

            static bool IsRegistered();
        };


        class GEX_API CompoundNode_Wrap: public Gex::CompoundNode
        {

        public:
            CompoundNode_Wrap();

            CompoundNode_Wrap(const CompoundNode& base) = delete;

            ~CompoundNode_Wrap() override;

        private:
            static bool registered;
            static PyClassRegistry registry;

        public:
            static bool RegisterPythonWrapper(pybind11::module_& mod,
                                              PyThreadState* state=nullptr);

            static bool IsRegistered();
        };

    }
}





#endif //GEX_NODE_WRAP_H
