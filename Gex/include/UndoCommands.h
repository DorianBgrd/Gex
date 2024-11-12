#ifndef GEX_UNDOCOMMANDS_H
#define GEX_UNDOCOMMANDS_H

#include "Gex.h"
#include "UndoStack.h"


namespace Gex::Undo
{
    class CreateNode: public UndoCmd
    {
        NodePtr parent;
        NodePtr node;

    public:
        CreateNode(const NodePtr& p,
                   const NodePtr& n);

        std::string Name() const override;

        void Undo() const override;

        void Redo() const override;
    };

    class DeleteNode: public UndoCmd
    {
        NodePtr node;
        NodePtr parent;
        std::vector<std::pair<AttributePtr, AttributeWkPtr>> connections;

    public:
        DeleteNode(const NodePtr& n,
                   const NodePtr& p);

        std::string Name() const override;

        void Undo() const override;

        void Redo() const override;
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

        void Undo() const override;

        void Redo() const override;
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

        void Undo() const override;

        void Redo() const override;
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

        void Undo() const override;

        void Redo() const override;
    };

}

#endif //GEX_UNDOCOMMANDS_H
