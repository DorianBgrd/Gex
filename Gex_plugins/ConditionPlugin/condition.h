#ifndef GEX_CONDITION_H
#define GEX_CONDITION_H

#include "Gex/include/Gex/Gex.h"

namespace Gex::Condition
{
    class ConditionNode: public Gex::Node
    {
        void InitAttributes() override;

        bool Evaluate(NodeAttributeData &context,
                      GraphContext &graphContext,
                      NodeProfiler &profiler) override;
    };


    class ConditionalScheduler: public Gex::CustomScheduler
    {
        ScheduledItemPtr Schedule(
                ScheduledGroupPtr& scheduled
        ) const override;
    };
}

#endif //GEX_CONDITION_H
