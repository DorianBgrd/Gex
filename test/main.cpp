#include <memory>
#include "iostream"
#include "Gex/include/Gex.h"


struct T: public std::enable_shared_from_this<T>
{
    int v = 0;

    bool operator < (const T& other) const
    {
        return v < other.v;
    }

    std::shared_ptr<T> Cp()
    {
        return shared_from_this();
    }
};


struct A
{
    double x = 0;
};


struct B: public A
{

};


int main(int argc, char** argv)
{
    Gex::Feedback res;
//    Gex::PluginLoader::LoadPlugin("D:\\WORK\\GEX\\Gex\\test\\TestPlugin\\testPlugin.json", &res);

    Gex::PluginLoader::LoadPlugin(
            "D:\\WORK\\GEX\\Gex\\cmake-build-debug\\plugins\\MathPlugin\\MathPlugin\\MathPlugin.json",
            &res);
    if (!res)
    {
        return 1;
    }

    auto addDoubles = Gex::NodeFactory::GetFactory()->CreateNode("Math/Add2Doubles", "BatchNode");

//    Gex::Undo::UndoStack::OpenGroup("Batch");
//
//    addDoubles->GetAttribute("Input1")->Set(5);
//    addDoubles->GetAttribute("Input2")->Set(10);
//
//    Gex::Undo::UndoStack::CloseGroup();
//
//    auto value1 = addDoubles->GetAttribute("Input1")->Get<double>();
//    auto value2 = addDoubles->GetAttribute("Input2")->Get<double>();
//
//    assert(value1 == 5);
//    assert(value2 == 10);
//
//    Gex::Undo::UndoStack::Undo();
//
//    value1 = addDoubles->GetAttribute("Input1")->Get<double>();
//    value2 = addDoubles->GetAttribute("Input2")->Get<double>();
//
//    assert(value1 == 0);
//    assert(value2 == 0);

//    std::set<T> s = {{0}, {1}, {2}};
//
//    auto s1 = *(s.begin()++);
//
//    s.erase(s1);
//    s1.v = 4;
//
//    s.insert(s1);

    std::shared_ptr<T> st = std::make_shared<T>();
    st->v = 6;

    Gex::BaseWkPtr<T> wt = st;

    auto sft = wt->Cp();

    std::shared_ptr<A> sharedA = std::make_shared<A>();
    std::shared_ptr<A> sharedB = std::make_shared<B>();
    std::shared_ptr<B> sharedB1 = std::make_shared<B>();

    std::cout << "sharedA : " << typeid(sharedA).hash_code() << std::endl;
    std::cout << "sharedB : " << typeid(sharedB.get()).hash_code() << std::endl;
    std::cout << "sharedB1 : " << typeid(sharedB1.get()).hash_code() << std::endl;

    return 0;
}
