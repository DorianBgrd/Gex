//
// Created by Dorian on 4/18/2023.
//

#ifndef NODELIBRARY2COMPLETE_GRAPH_H
#define NODELIBRARY2COMPLETE_GRAPH_H

#include "api.h"
#include "defs.h"
#include <vector>
#include <set>
#include <functional>
#include "rapidjson/document.h"

#include "Status.h"
#include "Profiler.h"
#include "Tsys/include/tsys.h"
#include "SafePtrs/safeptr.h"


namespace Gex
{

    class Node;
    class Graph;
    class Attribute;
    class NodeEvaluator;


    class GEX_API GraphContext
    {
        friend NodeEvaluator;
        std::vector<std::string> resources;
        ReadOnlyPropertyMap properties;

    public:
        GraphContext(Graph* graph);

        GraphContext(const GraphContext& other);

        void RegisterResource(std::string resource);

        std::vector<std::string> Resources() const;

        std::any PropertyValue(std::string name) const;
    };


    class GEX_API Property: USING_SAFE_PTR(Property)
    {
        std::any propValue;

    public:
        Property(const std::any& value=std::any());

        std::any GetAnyValue() const;

        template<typename T>
        Feedback GetValue(T& value) const
        {
            Feedback feedback;

            auto v = GetAnyValue();
            if (!v.has_value())
            {
                feedback.Set(Status::Failed, "No value found.");
                return feedback;
            }

            if (v.type().hash_code() == typeid(T).hash_code())
            {
                return value = std::any_cast<T>(v);
            }

            auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle<T>();
            if (!handler)
            {
                feedback.Set(Status::Failed, "Specified type is not registered.");
                return feedback;
            }

            if (!handler->CanConvertFrom(v))
            {
                feedback.Set(Status::Failed, "Current value is not convertible to specified type.");
                return feedback;
            }

            value = handler->ConvertFrom(v);
            feedback.Set(Status::Success, "");
            return feedback;
        }

        bool SetAnyValue(std::any value);

        template<typename T>
        bool SetValue(T value)
        {
            return SetAnyValue(std::make_any<T>(value));
        }
    };


    class GEX_API ReadOnlyProperty: public Property
    {
        SafePtr<Property> property;

    public:
        ReadOnlyProperty() = default;

        ReadOnlyProperty(Property* prop);

        ReadOnlyProperty(const ReadOnlyProperty& other);

        std::any GetAnyValue() const;

        template<typename T>
        Feedback GetValue(T& value) const
        {
            if (!property.valid())
            {
                Feedback failed;
                failed.Set(Status::Failed, "Current pointer is not valid.");
                return failed;
            }

            return property->GetValue<T>(value);
        }
    };


    class GEX_API Graph
    {
        friend GraphContext;

        NodeList nodes;
        AttributeList inputs;
        ScheduleNodeList scheduledNodes;
        bool valid = false;

        CallbackId invalidCbId = 0;
        CallbackId scheduleCbId = 0;
        InvalidateCallbacks invalidateCallbacks;
        ScheduleCallbacks scheduleCallbacks;

        PropertyMap properties;

    public:

        ~Graph();

        Node* CreateNode(std::string type, std::string name=std::string());

        bool AddNode(Node* node);

        Property* AddProperty(const std::string& name, const std::any& value=std::any());

        bool RemoveProperty(const std::string& name);

        Property* GetProperty(const std::string& name) const;

        ReadOnlyPropertyMap Properties() const;

    public:
        /**
         * Evaluation modifiers.
         */

        bool IsValid() const;

        void Invalidate();

        void Schedule();

        CallbackId AddInvalidateCallback(InvalidateCallback callback);

        void RemoveInvalidateCallback(CallbackId callback);

        void ClearInvalidateCallbacks();

        CallbackId AddScheduleCallback(ScheduleCallback callback);

        void RemoveScheduleCallback(CallbackId callback);

        void ClearScheduleCallbacks();

    public:
        NodeList DuplicateNodes(NodeList sources, bool copyLinks,
                                Node* parent, Graph* graph=nullptr);

    public:
        std::string UniqueName(const std::string& name) const;

        NodeList DuplicateNodes(NodeList sources,
                                          bool copyLinks=false);

        Gex::Node* ToCompound(NodeList sources, bool duplicate=false,
                              bool keepExternalConnections=true);

        bool RemoveNode(Node* node);

        bool RemoveNode(std::string node);

        Node* FindNode(std::string node) const;

        Attribute* FindAttribute(std::string attr) const;

        bool Serialize(rapidjson::Value& dict, rapidjson::Document& json) const;

        bool Deserialize(rapidjson::Value& dict);

        static Feedback CheckLoadStatus(rapidjson::Value& dict);

        bool Compute(Profiler& profile, unsigned int threads=1,
                     NodeCallback nodeStarted=nullptr,
                     NodeResCallback nodeDone=nullptr,
                     GraphCtxCallback evalDone=nullptr);

        void FinalizeEvaluation(const GraphContext& context) const;

        NodeList Nodes() const;

        std::vector<std::string> NodeNames() const;
    };
}



#endif //NODELIBRARY2COMPLETE_GRAPH_H
