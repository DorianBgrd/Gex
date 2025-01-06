#include "Nodes.h"
#include "Func.h"
#include "BezierFunc.h"


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



//std::string Gex::InputRel::BezierRel::Description() const
//{
//    return ("Creates a Bezier relationship "
//            "between two numeric attributes.");
//}
//
//
//void Gex::InputRel::BezierRel::InitAttributes()
//{
//    CreateAttribute<double>("input");
//
////    CreateAttributeFromValue("func", new LinearFunc());
//    CreateAttribute<BezierFuncPtr>("func");
//
//    CreateAttribute<double>("output", AttrValueType::Single,
//                            AttrType::Output);
//}
//
//bool Gex::InputRel::BezierRel::Evaluate(NodeAttributeData &context,
//                                        GraphContext &graphContext,
//                                        NodeProfiler &profiler)
//{
//    auto func = context.GetAttribute("input").GetValue<LinearFuncPtr>();
//
//    auto x = context.GetAttribute("input").GetValue<double>();
//    context.GetAttribute("output").SetValue(func->y(x));
//    return true;
//}