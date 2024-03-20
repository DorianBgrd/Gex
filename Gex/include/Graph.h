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
        PropertyWrappers properties;

    public:
        GraphContext(Graph* graph);

        GraphContext(const GraphContext& other);

        void RegisterResource(std::string resource);

        std::vector<std::string> Resources() const;

        PropertyWrapper Property(std::string name) const;
    };


    class GEX_API Property: USING_SAFE_PTR(Property)
    {
        size_t hash;
        std::any value;
        std::vector<Property*> properties;

        bool isArray = false;

    public:
        enum class Type
        {
            Input,
            Output
        };

    private:
        Type type;

    public:
        Property(std::any value, Type type=Type::Input);

        Property(size_t hash, Type type=Type::Input,
                 bool array=false);

        Type GetType() const;

        bool HasValue() const;

        bool IsArray() const;

        std::any GetValue() const;

        bool SetValue(std::any value);

        std::vector<std::any> GetArrayValues() const;

        bool SetArrayValues(std::vector<std::any> values);
    };


    class GEX_API PropertyWrapper
    {
        SafePtr<Property> property;

    public:
        PropertyWrapper() = default;

        PropertyWrapper(Property* prop);

        PropertyWrapper(const PropertyWrapper& other);

        std::any Get() const;

        std::vector<std::any> GetArrayValues() const;

        template<typename T>
        T GetValue(Feedback* success=nullptr) const
        {
            if (!property.valid())
            {
                if (success)
                    success->Set(Status::Failed, "Current pointer is not valid.");
                return T();
            }

            auto any = Get();

            auto destHash = typeid(T).hash_code();
            if (any.type().hash_code() == destHash)
                return std::any_cast<T>(any);

            auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(destHash);
            if (!handler->CanConvertFrom(any))
            {
                if (success)
                    success->Set(Status::Failed, "Destination and source "
                                                 "type are not compatible.");

                return T();
            }

            success->Set(Status::Success, "");

            auto res = handler->ConvertFrom(any, any);
            return std::any_cast<T>(res);
        }

        template<typename T>
        std::vector<T> ArrayValues(Feedback* success=nullptr) const
        {
            std::vector<T> res;
            for (auto any : GetArrayValues())
            {
                if (!typeid(T).hash_code())
                {

                }
            }
        }

        template<class T>
        bool Set(T value)
        {
            return SetValue(std::make_any<T>(value));
        }

        bool SetValue(const std::any& value);

        bool HasValue() const;

        bool IsArray() const;

        bool IsValid() const;
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

        PropertyWrappers Properties() const;

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
