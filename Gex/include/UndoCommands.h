#ifndef GEX_UNDOCOMMANDS_H
#define GEX_UNDOCOMMANDS_H

#include "Gex.h"
#include "UndoStack.h"


namespace Gex::Undo
{
    typedef std::function<bool(const Gex::NodePtr&)> NodeCB;


    class CreateNode: public UndoCmd
    {
        NodePtr parent;
        NodePtr node;

        NodeCB createCallback;
        NodeCB deleteCallback;

    public:
        CreateNode(const NodePtr& parent,
                   const NodePtr& node,
                   const NodeCB& createCallback,
                   const NodeCB& deleteCallback);

        std::string Name() const override;

        void Undo() override;

        void Redo() override;
    };

    class DeleteNode: public UndoCmd
    {
        NodePtr node;
        std::vector<std::pair<AttributePtr, AttributeWkPtr>> sourceConnections;
        std::vector<std::pair<AttributeWkPtr, AttributePtr>> destConnections;

    private:
        NodeCB removeNodeCb;
        NodeCB addNodeCb;

    public:
        DeleteNode(const NodePtr& node,
                   const NodeCB& removeCB,
                   const NodeCB& addNodeCb);

        DeleteNode(const DeleteNode& other);

        std::string Name() const override;

        void Undo() override;

        void Redo() override;
    };

    class ConnectAttr: public UndoCmd
    {
        NodePtr source;
        AttributePtr sourceAttr;
        NodePtr dest;
        AttributePtr destAttr;
        NodePtr previous;
        AttributePtr previousAttr;

    public:
        ConnectAttr(const AttributePtr& src,
                    const AttributePtr& dst,
                    const AttributePtr& pdst);

        std::string Name() const override;

        void Undo() override;

        void Redo() override;
    };

    class DisconnectAttr: public UndoCmd
    {
        NodePtr source;
        AttributePtr sourceAttr;
        NodePtr dest;
        AttributePtr destAttr;
        NodePtr previous;
        AttributePtr previousAttr;

    public:
        DisconnectAttr(const AttributePtr& src,
                       const AttributePtr& dst);

        std::string Name() const override;

        void Undo() override;

        void Redo() override;
    };

    class SetAttr: public UndoCmd
    {
        NodePtr node;
        AttributePtr attr;
        std::any value;
        std::any previous;

    public:
        SetAttr(const AttributePtr& at,
                const std::any& v,
                const std::any& p);

        std::string Name() const override;

        void Undo() override;

        void Redo() override;
    };

}

#endif //GEX_UNDOCOMMANDS_H
