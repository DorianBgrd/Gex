#include "condition.h"

#include "Tsys/include/Tsys/defaultTypes.h"


void Gex::Condition::ConditionNode::InitAttributes()
{
    CreateAttribute<TSys::AnyValue>(
            "Input1", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<TSys::AnyValue>(
            "Input2", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<bool>(
            "Condition", Gex::AttrValueType::Single,
            Gex::AttrType::Input
    );

    CreateAttribute<TSys::AnyValue>(
            "Output", Gex::AttrValueType::Single,
            Gex::AttrType::Output
    );
}


bool Gex::Condition::ConditionNode::Evaluate(
        NodeAttributeData &context,
        GraphContext &graphContext,
        NodeProfiler &profiler)
{
    auto condition = context.GetAttribute("Condition").GetValue<bool>();

    std::array<std::string, 2> alt = {"Input1", "Input2"};

    auto value = context.GetAttribute(
            alt.at((int)condition)
    ).GetValue<TSys::AnyValue>();

    return context.GetAttribute("Output").SetValue(value);
}

Gex::ScheduledItemPtr Gex::Condition::ConditionalScheduler::Schedule(
        ScheduledGroupPtr& scheduled
) const
{
    for (const auto& item : scheduled->Items())
    {
        if (auto nodeItem = std::dynamic_pointer_cast<Gex::ScheduledNode>(item))
        {
            auto node = nodeItem->GetNode();
            if (node->Type() == "Condition")
            {
                auto conditionBranch = GetScheduledBranch(
                    node, [](const ScheduledItemWkPtr& item)
                    {
                        auto sched = std::dynamic_pointer_cast<ScheduledNode>(item.ToShared());
                        if (!sched)
                        {
                            return true;
                        }

                        return sched->GetNode()->Type() != "Condition";
                    }
                );


                auto checkConditionBranch = [conditionBranch](const ScheduledItemWkPtr& item)
                {
                    if (std::find(conditionBranch.begin(), conditionBranch.end(), item) != conditionBranch.end())
                    {
                        return false;
                    }

                    auto sched = std::dynamic_pointer_cast<ScheduledNode>(item.ToShared());
                    if (!sched)
                    {
                        return true;
                    }

                    return sched->GetNode()->Type() != "Condition";
                };

                // Re-schedule input conditional branches...
                auto input1Branch = GetScheduledBranch(
                        node, checkConditionBranch
                );

                auto input2Branch = GetScheduledBranch(
                        node, checkConditionBranch
                );
            }
        }
    }

    return scheduled;
}