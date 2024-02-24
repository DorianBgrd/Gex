//
// Created by Dorian on 4/18/2023.
//

#ifndef NODELIBRARY2COMPLETE_GRAPH_H
#define NODELIBRARY2COMPLETE_GRAPH_H

#include "api.h"
#include <vector>
#include <set>
#include <functional>
#include "rapidjson/document.h"

#include "Status.h"
#include "Profiler.h"
#include "Tsys/include/tsys.h"


namespace Gex
{

    class Node;
    class Attribute;
    class NodeEvaluator;


    class GEX_API GraphContext
    {
        friend NodeEvaluator;
        std::vector<std::string> resources;

    public:
        GraphContext();

        GraphContext(const GraphContext& other);

        void RegisterResource(std::string resource);

        std::vector<std::string> Resources() const;
    };


    class GEX_API Graph
    {
        std::vector<Node*> nodes;
        std::vector<Attribute*> inputs;

    public:

        ~Graph();

        Node* CreateNode(std::string type, std::string name=std::string());

        bool RemoveNode(Node* node);

        bool RemoveNode(std::string node);

        Node* FindNode(std::string node) const;

        Attribute* FindAttribute(std::string attr) const;

        bool Serialize(rapidjson::Value& dict, rapidjson::Document& json) const;

        bool Deserialize(rapidjson::Value& dict);

        static Feedback CheckLoadStatus(rapidjson::Value& dict);

        bool Compute(GraphContext& context, Profiler& profile,
                     std::function<void(Node*)> nodeStarted=nullptr,
                     std::function<void(Node*, bool)> nodeDone=nullptr,
                     std::function<void(const GraphContext& context)> evalDone=nullptr) const;

        void FinalizeEvaluation(const GraphContext& context) const;

        std::vector<Node*> Nodes() const;

        std::vector<std::string> NodeNames() const;
    };
}



#endif //NODELIBRARY2COMPLETE_GRAPH_H
