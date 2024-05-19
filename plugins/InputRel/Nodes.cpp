#include "Nodes.h"
#include "Func.h"


std::string Gex::InputRel::LinearRel::Description() const
{
    return ("Creates a Linear relationship "
            "between two numeric attributes.");
}


void Gex::InputRel::LinearRel::InitAttributes()
{
    CreateAttribute<double>("input");

//    CreateAttributeFromValue("func", new LinearFunc());
    CreateAttribute<LinearFuncPtr>("func");

    CreateAttribute<double>("output", AttrValueType::Single,
                            AttrType::Output);
}

bool Gex::InputRel::LinearRel::Evaluate(NodeAttributeData &context,
                                        GraphContext &graphContext,
                                        NodeProfiler &profiler)
{
    auto func = context.GetAttribute("input").GetValue<LinearFuncPtr>();

    auto x = context.GetAttribute("input").GetValue<double>();
    context.GetAttribute("output").SetValue(func->y(x));
    return true;
}