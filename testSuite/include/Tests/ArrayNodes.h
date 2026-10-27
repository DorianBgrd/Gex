#ifndef GEX_ARRAYNODES_H
#define GEX_ARRAYNODES_H

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

        auto createArray = TS::NodeTest(
                "Arrays/Create",
                {
                    TS::AttrField{"Value", 0, 0},
                    TS::AttrField{"Value", 1, 1},
                },
                {
                    TS::AttrField{
                            "OutputArray",
                            std::vector<int>{0, 1}
                    }
                }
        );

        auto appendArray = TS::NodeTest(
                "Arrays/Append",
                {
                    TS::AttrField{"InputArray", std::vector<int>{0}},
                    TS::AttrField{"InputValue", 1}
                },
                {
                    TS::AttrField("OutputArray", std::vector<int>{0, 1})
                }
        );

        auto removeArray = TS::NodeTest(
                "Arrays/Remove",
                {
                    TS::AttrField("InputArray", std::vector<int>{0, 1, 2}),
                    TS::AttrField("Index", 1)
                },
                {
                    TS::AttrField("OutputArray", std::vector<int>{0, 2})
                }
        );

        auto insertArray = TS::NodeTest(
                "Arrays/Insert",
                {
                    TS::AttrField("InputArray", std::vector<int>{0, 2, 3}),
                    TS::AttrField("Index", 1),
                    TS::AttrField("Value", 1)
                },
                {
                    TS::AttrField("OutputArray", std::vector<int>{0, 1, 2, 3})
                }
        );

        auto arrayLength = TS::NodeTest(
                "Arrays/Length",
                {
                    TS::AttrField("InputArray", std::vector<int>{0, 1, 2, 3})
                },
                {
                    TS::AttrField("Length", 4)
                }
        );

        auto arrayIndex = TS::NodeTest(
                "Arrays/IndexValue",
                {
                    TS::AttrField("InputArray", std::vector<int>{0, 1, 2, 3}),
                    TS::AttrField("Index", 1)
                },
                {
                    TS::AttrField("Value", 1)
                }
        );

        auto arrayFirstItem = TS::NodeTest(
                "Arrays/FirstItem",
                {
                        TS::AttrField("InputArray", std::vector<int>{0, 1, 2, 3})
                },
                {
                        TS::AttrField("Value", 0)
                }
        );

        auto arrayLastItem = TS::NodeTest(
                "Arrays/LastItem",
                {
                        TS::AttrField("InputArray", std::vector<int>{0, 1, 2, 3})
                },
                {
                        TS::AttrField("Index", 3),
                        TS::AttrField("Value", 3)
                }
        );

        auto arraySlice = TS::NodeTest(
                "Arrays/Slice",
                {
                    TS::AttrField("InputArray", std::vector<int>{0, 1, 2, 3}),
                    TS::AttrField("Start", 1),
                    TS::AttrField("End", 2)
                },
                {
                    TS::AttrField("OutputArray", std::vector<int>{1, 2})
                }
        );

        TS::TestNodes(
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
