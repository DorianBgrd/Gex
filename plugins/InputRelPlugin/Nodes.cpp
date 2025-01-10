#include "Nodes.h"
#include "Func.h"
#include "BezierFunc.h"

#include "Tsys/include/defaultTypes.h"


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
    auto func = context.GetAttribute("Func").GetValue<LinearFuncPtr>();

    auto x = context.GetAttribute("Input").GetValue<double>();
    context.GetAttribute("Output").SetValue(func->GetValue(x));
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

    TSys::Enum methods({
           {0, "CubicRoots"},
           {1, "Iterative"}
        }
    );

    methods.SetCurrentIndex(1);

    CreateAttribute<TSys::Enum>("Method", AttrValueType::Single,
                                AttrType::Input)->SetDefaultValue(methods);

    CreateAttribute<double>("Output", AttrValueType::Single,
                            AttrType::Output);
}

bool Gex::InputRel::BezierRel::Evaluate(NodeAttributeData &context,
                                        GraphContext &graphContext,
                                        NodeProfiler &profiler)
{
    auto func = context.GetAttribute("Func").GetValue<BezierFuncPtr>();

    auto method = context.GetAttribute("Method").GetValue<TSys::Enum>();
    func->SetMethod(BezierFunc::InterpMethod(method.CurrentIndex()));

    auto x = context.GetAttribute("Input").GetValue<double>();
    context.GetAttribute("Output").SetValue(func->GetValue(x));
    return true;
}