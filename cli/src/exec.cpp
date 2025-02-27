#include "cli/include/exec.h"


int Exec::ExecuteGraph(int argc, char** argv, int start) {
    Parser parser;

    parser.AddFlag("-iv", "--input-values", 1,
                   "Overrides attribute input value formatted as a json dict"
                   "using {attribute: value}.");

    parser.AddFlag("-tn", "--thread-number", 1,
                   "Overrides execution thread number, default : 1");

    parser.AddFlag("-v", "--verbose", 0, "Execution verbosity.");

    parser.Parse(argc, argv, start);

    int thnb = 1;
    bool verbose = parser.FlagFound("-v");

    if (parser.FlagFound("-tn"))
    {
        try
        {
            thnb = std::stoi(parser.FlagResult("-tn"));
        }
        catch (std::invalid_argument&)
        {
            std::cout << "Warning: invalid number of "
                         "thread specified" << std::endl;
        }
    }

    auto files = parser.Arguments();
    if (files.empty())
    {
        std::cerr << "At least one gex file should be specified." << std::endl;
        return 4;
    }

    std::vector<Gex::NodePtr> gexGraphs;
    for (auto filepath: files)
    {
        Gex::Feedback result;
        auto graph = Gex::LoadGraph(filepath, &result);

        if (!result)
        {
            std::string message = result.message;
            std::cout << "Failed loading file " << filepath << " : " << message << std::endl;
            continue;
        }

        gexGraphs.push_back(graph);
    }

    if (gexGraphs.empty())
    {
        std::cerr << "No valid file specified." << std::endl;
    }

    std::function<void(const Gex::NodePtr&)> startCallback;
    std::function<void(const Gex::NodePtr&, bool)> endCallback;

    if (verbose)
    {
        startCallback = &VerboseNodeStart;
        endCallback = &VerboseNodeEnd;
    }

    bool globalSuccess = true;
    for (auto graph: gexGraphs)
    {
        std::string attrValues = parser.FlagResult("-iv");
        if (!attrValues.empty())
        {
            rapidjson::Document json;
            json.Parse(attrValues.c_str());

            if (json.IsObject())
            {
                auto obj = json.GetObject();
                for (auto member = obj.MemberBegin(); member != obj.MemberEnd(); member++)
                {
                    std::string attributeName = member->name.GetString();
                    auto attr = graph->GetAttribute(attributeName);
                    if (!attr)
                    {
                        std::cout << "Warning: could not find attribute "
                                  << attributeName << " on node "
                                  << graph->Name() << std::endl;
                        continue;
                    }

                    ResolveJsonValue(attr.ToShared(), member->value);
                }
            }
            else
            {
                std::cout << "Warning : Malformed attribute value description, skipping." << std::endl;
            }


        }

        auto scheduled = graph->ToScheduledNodes();
        Gex::GraphContext context;

        auto profiler = Gex::MakeProfiler();
        if (!graph->Run(profiler, thnb, startCallback, endCallback))
        {
            globalSuccess = false;
        }
    }

    return !globalSuccess;
}


void Exec::VerboseNodeEnd(Gex::NodePtr node, bool success)
{
    std::string msg = "Successfully evaluated";
    if (!success)
        msg = "Failed evaluating";

    std::cout << msg << "  " << node->Name() << std::endl;
}


void Exec::VerboseNodeStart(Gex::NodePtr node)
{
    std::cout << "Starting evaluating " << node->Name() << std::endl;
}


bool Exec::ResolveJsonValue(Gex::AttributePtr attr, rapidjson::Value& value)
{
    if (value.IsArray())
    {
        unsigned int i = 0;
        for (auto iter = value.Begin(); iter != value.End(); iter++)
        {
            attr->CreateIndex(i);
            auto indexAttribute = attr->GetIndexAttribute(i);

            ResolveJsonValue(indexAttribute.ToShared(), *iter);

            i++;
        }
    }
    else if (value.IsDouble())
    {
        return attr->Set(value.GetDouble());
    }
    else if (value.IsInt())
    {
        return attr->Set(value.GetInt());
    }
    else if (value.IsBool())
    {
        return attr->Set(value.GetBool());
    }

    return false;
}

