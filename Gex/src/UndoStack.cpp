#include "Gex/include/UndoStack.h"


Gex::Undo::UndoCmdGroup::UndoCmdGroup(
        const std::string& n,
        const UndoGrpPtr& p)
{
    name = n;
    parent = p;
}


std::string Gex::Undo::UndoCmdGroup::Name() const
{
    return name;
}


void Gex::Undo::UndoCmdGroup::AddUndo(const UndoPtr& command)
{
    group.push_back(command);
}


void Gex::Undo::UndoCmdGroup::Undo()
{
    for (const auto undo : group)
    {
        undo->Undo();
    }
}


void Gex::Undo::UndoCmdGroup::Redo()
{
    for (const auto undo : group)
    {
        undo->Redo();
    }
}


Gex::Undo::UndoGrpWkPtr Gex::Undo::UndoCmdGroup::ParentGroup() const
{
    return parent;
}


std::vector<Gex::Undo::UndoPtr>& Gex::Undo::UndoCmdGroup::Undos()
{
    return group;
}


Gex::Undo::UndoStack* Gex::Undo::UndoStack::instance = nullptr;


bool Gex::Undo::UndoStack::_AddUndo(UndoPtr undo)
{
    if (!active)
        return false;

    if (currentGrp)
    {
        currentGrp->AddUndo(undo);
    }
    else
    {
        undos.push_back(undo);
        redos.clear();

        if (undos.size() > stackSize)
            undos.erase(undos.begin());
    }

    return true;
}


bool Gex::Undo::UndoStack::_Undo()
{
    if (undos.empty())
        return false;

    undoing = true;

    while (currentGrp)
        CloseGroup();

    auto rb = undos.rbegin();
    auto undo = (*rb);
    undo->Undo();

    undos.pop_back();
    redos.push_back(undo);

    undoing = false;
    return true;
}


bool Gex::Undo::UndoStack::_Redo()
{
    if (redos.empty())
        return false;

    redoing = true;

    auto rb = redos.rbegin();
    auto undo = (*rb);
    undo->Redo();

    redos.pop_back();
    undos.push_back(undo);

    redoing = false;
    return true;
}


bool Gex::Undo::UndoStack::_IsUndoing() const
{
    return undoing;
}

bool Gex::Undo::UndoStack::_IsRedoing() const
{
    return redoing;
}


void Gex::Undo::UndoStack::_Clear()
{
    undos.clear();
    redos.clear();
}


bool Gex::Undo::UndoStack::_IsActive()
{
    return active;
}


void Gex::Undo::UndoStack::_SetActive(bool a)
{
    active = a;
}


void Gex::Undo::UndoStack::_Disable()
{
    _SetActive(false);
}


void Gex::Undo::UndoStack::_Enable()
{
    _SetActive(true);
}


void Gex::Undo::UndoStack::_OpenGroup(const std::string& name)
{
    auto grp = std::make_shared<UndoCmdGroup>(
            name, currentGrp);

    _AddUndo(grp);

    currentGrp = grp;
}


void Gex::Undo::UndoStack::_CloseGroup()
{
    if (!currentGrp)
        return;

    auto parent = currentGrp->ParentGroup();

    UndoGrpPtr swap;
    if (!parent.expired())
    {
        swap = parent.lock();
    }

    currentGrp = swap;
}


Gex::Undo::UndoStack* Gex::Undo::UndoStack::GetInstance()
{
    if (!instance)
        instance = new UndoStack();

    return instance;
}


bool Gex::Undo::UndoStack::AddUndo(UndoPtr undo)
{
    return GetInstance()->_AddUndo(undo);
}


bool Gex::Undo::UndoStack::Undo()
{
    return GetInstance()->_Undo();
}


bool Gex::Undo::UndoStack::Redo()
{
    return GetInstance()->_Redo();
}


bool Gex::Undo::UndoStack::IsUndoing()
{
    return GetInstance()->_IsUndoing();
}


bool Gex::Undo::UndoStack::IsRedoing()
{
    return GetInstance()->_IsRedoing();
}


bool Gex::Undo::UndoStack::IsDoing()
{
    return (IsUndoing() || IsRedoing());
}


void Gex::Undo::UndoStack::Clear()
{
    GetInstance()->_Clear();
}


void Gex::Undo::UndoStack::OpenGroup(
        const std::string& name)
{
    GetInstance()->_OpenGroup(name);
}


void Gex::Undo::UndoStack::CloseGroup()
{
    GetInstance()->_CloseGroup();
}


void Gex::Undo::UndoStack::SetActive(bool a)
{
    GetInstance()->_SetActive(a);
}


void Gex::Undo::UndoStack::Disable()
{
    GetInstance()->_Disable();
}


void Gex::Undo::UndoStack::Enable()
{
    GetInstance()->_Enable();
}


bool Gex::Undo::UndoStack::IsActive()
{
    return GetInstance()->_IsActive();
}
