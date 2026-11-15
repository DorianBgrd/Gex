#ifndef GEX_ARRAYNODES_H
#define GEX_ARRAYNODES_H

#include <memory>

#include "TS/TestNodes.h"

#include "Gex/PluginLoader.h"


namespace TestSuite::Array
{
    void TestArrayNodes()
    {
        Gex::Feedback loadfeedback;
        Gex::PluginLoader::LoadPlugin("ArraysPlugin", &loadfeedback);

        if (!loadfeedback)
        {
            // print error.
        }


        auto createArray = std::make_shared<TS::NodeTest>(
                "Arrays/Create",
                std::vector<TS::AttrField>{
                    TS::AttrField{"Value", 0, 0},
                    TS::AttrField{"Value", 1, 1},
                },
                std::vector<TS::AttrField>{
                    TS::AttrField{
                            "OutputArray",
                            std::vector<int>{0, 1}
                    }
                }
        );

        auto appendArray = std::make_shared<TS::NodeTest>(
                "Arrays/Append",
                std::vector<TS::AttrField>{
                    TS::AttrField{"InputArray", std::vector<int>{0}},
                    TS::AttrField{"InputValue", 1}
                },
                std::vector<TS::AttrField>{
                    TS::AttrField("OutputArray", std::vector<int>{0, 1})
                }
        );

        auto removeArray = std::make_shared<TS::NodeTest>(
                "Arrays/Remove",
                std::vector<TS::AttrField>{
                    TS::AttrField("InputArray", std::vector<int>{0, 1, 2}),
                    TS::AttrField("Index", 1)
                },
                std::vector<TS::AttrField>{
                    TS::AttrField("OutputArray", std::vector<int>{0, 2})
                }
        );

        auto insertArray = std::make_shared<TS::NodeTest>(
                "Arrays/Insert",
                std::vector<TS::AttrField>{
                    TS::AttrField("InputArray", std::vector<int>{0, 2, 3}),
                    TS::AttrField("Index", 1),
                    TS::AttrField("Value", 1)
                },
                std::vector<TS::AttrField>{
                    TS::AttrField("OutputArray", std::vector<int>{0, 1, 2, 3})
                }
        );

        auto arrayLength = std::make_shared<TS::NodeTest>(
                "Arrays/Length",
                std::vector<TS::AttrField>{
                    TS::AttrField("InputArray", std::vector<int>{0, 1, 2, 3})
                },
                std::vector<TS::AttrField>{
                    TS::AttrField("Length", 4)
                }
        );

        auto arrayIndex = std::make_shared<TS::NodeTest>(
                "Arrays/IndexValue",
                std::vector<TS::AttrField>{
                    TS::AttrField("InputArray", std::vector<int>{0, 1, 2, 3}),
                    TS::AttrField("Index", 1)
                },
                std::vector<TS::AttrField>{
                    TS::AttrField("Value", 1)
                }
        );

        auto arrayFirstItem = std::make_shared<TS::NodeTest>(
                "Arrays/FirstItem",
                std::vector<TS::AttrField>{
                        TS::AttrField("InputArray", std::vector<int>{0, 1, 2, 3})
                },
                std::vector<TS::AttrField>{
                        TS::AttrField("Value", 0)
                }
        );

        auto arrayLastItem = std::make_shared<TS::NodeTest>(
                "Arrays/LastItem",
                std::vector<TS::AttrField>{
                        TS::AttrField("InputArray", std::vector<int>{0, 1, 2, 3})
                },
                std::vector<TS::AttrField>{
                        TS::AttrField("Index", 3),
                        TS::AttrField("Value", 3)
                }
        );

        auto arraySlice = std::make_shared<TS::NodeTest>(
                "Arrays/Slice",
                std::vector<TS::AttrField>{
                    TS::AttrField("InputArray", std::vector<int>{0, 1, 2, 3}),
                    TS::AttrField("Start", 1),
                    TS::AttrField("End", 2)
                },
                std::vector<TS::AttrField>{
                    TS::AttrField("OutputArray", std::vector<int>{1, 2})
                }
        );

        TS::RunTestCases(
            {
                    createArray,
                    appendArray,
                    removeArray,
                    insertArray,
                    arrayLength,
                    arrayIndex,
                    arrayFirstItem,
                    arrayLastItem,
                    arraySlice
            }
        );
    }
}

#endif //GEX_ARRAYNODES_H
