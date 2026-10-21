#include "Nodes.h"
#include "Func.h"
#include "BezierFunc.h"

#include "Tsys/defaultTypes.h"


std::string Gex::InputRel::LinearRel::Description() const
{
    return ("Creates a Linear relationship "
            "between two numeric attributes.");
}


void Gex::InputRel::LinearRel::InitAttributes()
{
    CreateAttribute<double>("Input");

//    CreateAttributeFromValue("func", new LinearFunc());
    CreateAttribute<LinearFuncPtr>("Func");

    CreateAttribute<double>("Output", AttrValueType::Single,
                            AttrType::Output);
}

bool Gex::InputRel::LinearRel::Evaluate(NodeAttributeData &context,
                                        GraphContext &graphContext,
                                        NodeProfiler &profiler)
{
    auto func = context.GetAttribute("Func").Get<LinearFuncPtr>();

    auto x = context.GetAttribute("Input").Get<double>();
    context.GetAttribute("Output").Set(func->GetValue(x));
    return true;
}



std::string Gex::InputRel::BezierRel::Description() const
{
    return ("Creates a Bezier relationship "
            "between two numeric attributes.");
}


void Gex::InputRel::BezierRel::InitAttributes()
{
    CreateAttribute<double>("Input");

//    CreateAttributeFromValue("func", new LinearFunc());
    CreateAttribute<BezierFuncPtr>("Func");

    std::vector<std::string> stringMethods = {
            "CubicRoots", "Iterative"
    };

    TSys::Enum methods(stringMethods);

    methods.Set(1);

    CreateAttribute<TSys::Enum>("Method", AttrValueType::Single,
                                AttrType::Input)->SetDefaultValue(methods);

    CreateAttribute<double>("Output", AttrValueType::Single,
                            AttrType::Output);
}

bool Gex::InputRel::BezierRel::Evaluate(NodeAttributeData &context,
                                        GraphContext &graphContext,
                                        NodeProfiler &profiler)
{
    auto func = context.GetAttribute("Func").Get<BezierFuncPtr>();

    auto method = context.GetAttribute("Method").Get<TSys::Enum>();
    func->SetMethod(BezierFunc::InterpMethod(method.GetCurrentIndex()));

    auto x = context.GetAttribute("Input").Get<double>();
    context.GetAttribute("Output").Set(func->GetValue(x));
    return true;
}