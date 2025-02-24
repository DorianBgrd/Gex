#ifndef GEX_UNDOSTACK_H
#define GEX_UNDOSTACK_H

#include <string>
#include <stack>
#include <vector>
#include <memory>
#include "api.h"


namespace Gex::Undo
{
    class GEX_API UndoCmd
    {
    public:
        virtual std::string Name() const = 0;
        virtual void Undo() = 0;
        virtual void Redo() = 0;
    };


    typedef std::shared_ptr<UndoCmd> UndoPtr;
    typedef std::weak_ptr<UndoCmd> UndoWkPtr;

    class UndoCmdGroup;

    typedef std::shared_ptr<UndoCmdGroup> UndoGrpPtr;
    typedef std::weak_ptr<UndoCmdGroup> UndoGrpWkPtr;


    class GEX_API UndoCmdGroup: public UndoCmd
    {
    private:
        std::vector<UndoPtr> group;
        UndoGrpWkPtr parent;
        std::string name;

    public:
        UndoCmdGroup() = default;

        UndoCmdGroup(const std::string& name,
                     const UndoGrpPtr& parent=nullptr);

        std::string Name() const override;

        void AddUndo(const UndoPtr& command);

        void Undo() override;

        void Redo() override;

        UndoGrpWkPtr ParentGroup() const;

        std::vector<UndoPtr>& Undos();
    };


    class GEX_API UndoStack
    {
    private:
        std::vector<UndoPtr> undos;
        std::vector<UndoPtr> redos;
        bool active = true;
        bool undoing = false;
        bool redoing = false;
        unsigned int stackSize = 50;
        UndoGrpPtr currentGrp;

        UndoStack() = default;

    public:
        bool _AddUndo(UndoPtr undo);

        bool _Undo();

        bool _Redo();

        bool _IsUndoing() const;

        bool _IsRedoing() const;

        void _Clear();

        bool _IsActive();

        void _SetActive(bool active);

        void _Disable();

        void _Enable();

        void _OpenGroup(const std::string& name="UndoGroup");

        void _CloseGroup();

    private:
        static UndoStack* instance;

    public:
        static UndoStack* GetInstance();

    public:
        static bool AddUndo(UndoPtr undo);

        static bool Undo();

        static bool Redo();

        static bool IsUndoing();

        static bool IsRedoing();

        static bool IsDoing();

        static bool IsActive();

        static void SetActive(bool active);

        static void Disable();

        static void Enable();

        static void Clear();

        static void OpenGroup(const std::string& name="UndoGroup");

        static void CloseGroup();
    };


    template<class T, typename... Args>
    UndoPtr GEX_API CreateUndo(Args... args)
    {
        return std::make_shared<T>(args...);
    }

}

#endif //GEX_UNDOSTACK_H
