#include "TestAttributes.h"

#include "tests.h"

void Test::TestAttributes()
{
    Gex::CompoundNodePtr compound = Gex::Node::ConvertTo<Gex::CompoundNode>(
            Gex::NodeFactory::GetFactory()->CreateNode("CompoundNode", "Compound")
    );

    auto unexistingAttribute = compound->GetAttribute("Invalid");

    ASSERT_FALSE("Unexisting attribute.", static_cast<bool>(unexistingAttribute),
                 "Trying to get an unexisting attribute should return an invalid "
                 "Attribute weak ptr.");

    auto existingAttribute = compound->GetAttribute("Next");

    ASSERT_TRUE("Existing attribute.",  static_cast<bool>(existingAttribute),
                "Trying to get an existing attribute should return a valid "
                "Attribute weak ptr.");

    auto newAttribute = compound->CreateAttributeFromTypeName(
            "TestAttribute", "String", Gex::AttrValueType::Single,
            Gex::AttrType::Input);

    ASSERT_TRUE("Creating user defined attribute", static_cast<bool>(newAttribute),
                "Failed creating a new attribute with type string.");

    bool failed = newAttribute->Set('c');

    ASSERT_FALSE("Setting an invalid typed value", static_cast<bool>(failed),
                 "Trying to set an int value on a string attribute should return "
                 "a failed status.");

    std::string value1 = "StringValue";
    bool success = newAttribute->Set(value1);

    ASSERT_TRUE("Setting a valid typed value.", static_cast<bool>(success),
                "Trying to set a string value on a string attribute should return "
                "a success status.");

    auto value2 = newAttribute->Get<std::string>();

    ASSERT_EQUAL("Comparing set and get value.", value1, value2,
                 "Compared get and set value should be equal.");

    // Create sub nodes.
    auto noOpNode1 = compound->CreateNode("CompoundNode", "NoOp1");
    auto outString = noOpNode1->CreateAttributeFromTypeName(
            "OutString", "String",
            Gex::AttrValueType::Single,
            Gex::AttrType::Static);

    auto noOpNode2 = compound->CreateNode("CompoundNode", "NoOp1");
    auto inString = noOpNode1->CreateAttributeFromTypeName(
            "InString", "String",
            Gex::AttrValueType::Single,
            Gex::AttrType::Static);

    bool stringConnected = inString->ConnectSource(outString);

    ASSERT_TRUE("Connecting 2 string attributes", stringConnected,
                "Connection between the attributes should succeed.");

    ASSERT_FALSE("Trying to set InString value.", inString->Set(std::string("value")),
                 "Setting value on a connected attribute should fail.");

    // Decide which value should be shown when connected ?
    outString->Set("StrVal");

//    auto testValue = inString->Get<std::string>();
//
//    ASSERT_EQUAL("Getting a connected attribute value.", outString->Get<std::string>(), testValue,
//                 "Getting an attribute value while it is connected should refresh its value.");

    auto inValue = compound->CreateAttributeFromTypeName(
            "inValue", "String",
            Gex::AttrValueType::Single,
            Gex::AttrType::Input);

    inValue->SetInternal(false);

    ASSERT_FALSE("Trying to connect a compound child to non internal input.",
                 inString->ConnectSource(inValue),
                 "This type of connection should fail !");

    auto outValue = compound->CreateAttributeFromTypeName(
            "outValue", "String",
            Gex::AttrValueType::Single,
            Gex::AttrType::Output);

    outValue->SetInternal(false);

    ASSERT_FALSE("Trying to connect a compound child to non internal output.",
                 outValue->ConnectSource(outString),
                 "This type of connection should fail !");

    inValue->SetInternal(true);

    ASSERT_TRUE("Trying to connect a compound child to internal input.",
                inString->ConnectSource(inValue),
                "This type of connection should succeed !");

    outValue->SetInternal(true);

    ASSERT_TRUE("Trying to connect a compound child to internal output.",
                outValue->ConnectSource(outString),
                "This type of connection should succeed !");

    auto inputString = noOpNode1->CreateAttributeFromTypeName(
            "InputString", "String",
            Gex::AttrValueType::Single,
            Gex::AttrType::Input);

    ASSERT_FALSE("Trying to connect a compound child input to internal output.",
                 inputString->ConnectSource(outValue),
                 "This type of connection should fail !");

    auto outputString = noOpNode1->CreateAttributeFromTypeName(
            "OutputString", "String",
            Gex::AttrValueType::Single,
            Gex::AttrType::Output);

    ASSERT_FALSE("Trying to connect a compound child output to internal input.",
                 outputString->ConnectSource(inValue),
                 "This type of connection should fail !");
}