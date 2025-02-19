#include "TestNodes.h"
#include "tests.h"



void Test::TestNodes()
{
    Gex::NodePtr node = Gex::NodeFactory::GetFactory()->CreateNode(
            "CompoundNode", "TestGraphNode"
    );

    Gex::CompoundNodePtr compound = Gex::Node::ConvertTo<Gex::CompoundNode>(node);

    ASSERT_TRUE("Compound node was created", static_cast<bool>(compound),
                "Failed converting to compound node.");

    Gex::NodeWkPtr invalidNode = compound->GetNode("InvalidNode");

    ASSERT_FALSE("Invalid node query", static_cast<bool>(invalidNode),
                 "Getting an unexisting node should return an invalid node.");

    Gex::NodeWkPtr node1 = compound->CreateNode("CompoundNode", "SubNode1");

    ASSERT_TRUE("Creating a new node.", static_cast<bool>(node1),
                "Failed creating a new node with a default 'CompoundNode' type.");

    Gex::Undo::UndoStack::Undo();

    ASSERT_FALSE("Undoing creating new node.", compound->HasNode("SubNode1"),
                 "Undoing create node and adding it to a compound node should "
                 "destroy it due to no left strong reference.");

    Gex::Undo::UndoStack::Redo();
    node1 = compound->GetNode("SubNode1");

    ASSERT_TRUE("Redoing creating a new node.", static_cast<bool>(node1),
                "Failed recreating the new node using the Redo() command.");

    compound->RemoveNode(node1->Name());

    ASSERT_FALSE("Removing a node from compound.", compound->HasNode(node1),
                 "Removing node from a compound failed.");

    Gex::Undo::UndoStack::Undo();

    ASSERT_TRUE("Undoing removing node from compound.", compound->HasNode(node1),
                "Undoing the remove node from compound action did not re-add it to the compound.");

    Gex::Undo::UndoStack::Redo();

    ASSERT_FALSE("Redoing remove a node from compound.", compound->HasNode(node1),
                 "Redoing the remove node from compound action did not re-remove it from the compound.");

    Gex::Undo::UndoStack::Clear();

    ASSERT_FALSE("Clearing undo stack.", static_cast<bool>(node1),
                 "Clearing undo stack after having removed from compound "
                 "should destroy all its references.");
}