#ifndef GEX_SCHEDULING_H
#define GEX_SCHEDULING_H

#include "api.h"
#include "defs.h"

#include "Profiler.h"
#include <typeindex>
#include <mutex>


namespace Gex {
    class Node;

    class GraphContext;

    typedef std::function<bool(GraphContext &, const Profiler&, const std::string&)> Evaluation;

    class EvalFunction
    {
        friend ScheduledItem;

        ScheduledItemPtr associatedItem;
        Evaluation func;

        void Configure(const ScheduledItemPtr& item,
                       const Evaluation& call)
        {
            associatedItem = item;
            func = call;
        }

    public:
        EvalFunction() = default;

        ScheduledItemPtr Item() const
        {
            return associatedItem;
        }

        bool operator()(GraphContext& g, const Profiler& p, const std::string& n)
        {
            return func && func(g, p, n);
        }

        operator bool() const
        {
            return associatedItem && func;
        }
    };

    class GEX_API ScheduledItem : public std::enable_shared_from_this<ScheduledItem> {
        ScheduledItemWkList previous;
        ScheduledItemWkList next;

        CallbackId index = 0;
        std::map<CallbackId, ScheduleItemCallback> callbacks;
        bool evaluated = false;

    public:
        explicit ScheduledItem() = default;

        bool ShouldBeEvaluated() const;

//        virtual bool Evaluate(GraphContext &context,
//                              NodeProfiler &profiler);

//        bool Compute(GraphContext &context,
//                     NodeProfiler &profiler);

        bool Evaluated() const;

        CallbackId RegisterCallback(const ScheduleItemCallback &);

        bool DeregisterCallback(CallbackId index);

        void LinkPrevious(const ScheduledItemWkPtr &prev);

        void UnlinkPrevious(const ScheduledItemWkPtr &prev);

        bool Acquire(EvalFunction& function);

        virtual bool Advance(Evaluation &function);

        virtual std::vector<ScheduledItemPtr> Items() const;

    protected:
        void LinkFuture(const ScheduledItemWkPtr &future);

        void UnlinkFuture(const ScheduledItemWkPtr &future);
    };


    class GEX_API ScheduledNode : public ScheduledItem {
        NodeWkPtr node;

    public:
        explicit ScheduledNode() = default;

        explicit ScheduledNode(const NodeWkPtr &node);

//        bool Evaluate(GraphContext &context,
//                      NodeProfiler &profiler) override;

        NodeWkPtr GetNode() const;

        bool Advance(Evaluation& function) override;

        bool operator==(const ScheduledNode &other) const;

        bool operator==(const Gex::NodePtr &other) const;

        bool operator==(const Gex::NodeWkPtr &other) const;

        bool operator==(const ScheduledNode *other) const;
    };


    class GEX_API ScheduledGroup : public ScheduledItem {
        std::vector<ScheduledItemPtr> items;
        std::vector<ScheduledItemPtr>::iterator current;
        std::vector<ScheduledItemPtr>::iterator end;

    public:
        explicit ScheduledGroup() = default;

        explicit ScheduledGroup(const std::vector<ScheduledItemPtr> &nodes);

        bool Advance(Evaluation& function) override;

        std::vector<ScheduledItemPtr> Items() const override;
    };


    typedef std::shared_ptr<ScheduledGroup> ScheduledGroupPtr;


    class GEX_API CustomScheduler {
    public:
        CustomScheduler() = default;

        virtual ScheduledItem Schedule(
                const ScheduledGroupPtr &scheduled
        ) const = 0;
    };

    typedef std::shared_ptr<CustomScheduler> CustomSchedulerPtr;


    typedef std::function<CustomSchedulerPtr()> CustomSchedulerDelegate;

    struct GEX_API CustomSchedulerBuilder {
        std::type_index index;
        CustomSchedulerDelegate func;

        bool operator==(const CustomSchedulerBuilder &other) const {
            return other.index == index;
        }
    };

    GEX_API
    ScheduledGroupPtr ScheduleGraph(const NodeList& nodes);


    class GEX_API Scheduler
    {
        std::vector<CustomSchedulerPtr> schedulers;

    public:
        Scheduler(const std::vector<CustomSchedulerPtr>& customSchedulers);

        ScheduledGroupPtr Schedule(const NodeList& nodes) const;
    };


    class GEX_API SchedulerRegistry
    {
        static std::vector<CustomSchedulerBuilder> registry;
        static std::mutex lock;

    public:
        template<typename T>
        static void RegisterCustomScheduler()
        {
            std::lock_guard<std::mutex> l(lock);

            registry.emplace_back(
                std::type_index(typeid(T)),
                [](){return std::make_shared<T>();}
            );
        };

        template<typename T>
        static void DeregisterCustomScheduler()
        {
            std::lock_guard<std::mutex> l(lock);

            auto iter = std::find(
                    registry.begin(), registry.end(),
                  std::type_index(typeid(T))
            );

            if (iter == registry.end())
                return;

            registry.erase(iter);
        }

        static Scheduler CreateScheduler();
    };



//    GEX_API
//    ScheduleNodePtrList ScheduleNodes(const NodeList& nodes, bool expand = true);
//
//    GEX_API
//    ScheduleNodePtrList::iterator FindScheduledNode(ScheduleNodePtrList list,
//                                                    const Gex::NodePtr& node);
//
//    GEX_API
//    ScheduleNodePtrList CopyScheduledNodes(const ScheduleNodePtrList& nodes);
//
//    GEX_API
//    ScheduleNodePtrList SubScheduledNodes(const ScheduleNodePtrList& list,
//                                          const Gex::NodePtr& node);
}

#endif //GEX_SCHEDULING_H
