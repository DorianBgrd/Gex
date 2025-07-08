#include "ui/include/ViewContexts.h"


Gex::Ui::BaseGraphView* Gex::Ui::ViewContext::CurrentView() const
{
    return currentView;
}


void Gex::Ui::ViewContext::Activate(BaseGraphView* view)
{
    currentView = view;

    Setup(currentView);
}


void Gex::Ui::ViewContext::Finalize(BaseGraphView* scene)
{

}


Gex::Ui::ViewContexts::ViewContexts(BaseGraphView* view)
{
    currentView = view;
}


Gex::Ui::ViewContexts::~ViewContexts()
{
    for (const auto& p : contexts)
    {
        delete p.second;
    }
}


std::string Gex::Ui::ViewContexts::CurrentContext() const
{
    return currentContext;
}


bool Gex::Ui::ViewContexts::StartContext(const std::string& context)
{
    if (currentContext == context)
        return false;

    auto iter = contexts.find(context);
    if (iter == contexts.end())
    {
        return false;
    }

    currentContext = context;
    currentContextPtr = iter->second;

    currentContextPtr->Activate(currentView);

    return true;
}


bool Gex::Ui::ViewContexts::EndContext(const std::string& context)
{
    auto iter = contexts.find(context);
    if (iter == contexts.end())
    {
        return false;
    }

    iter->second->Finalize(currentView);
    return true;
}


bool Gex::Ui::ViewContexts::SetCurrentContext(const std::string& context)
{
    if (!StartContext(context))
        return false;

    contextsStack.push(context);

    return true;
}


bool Gex::Ui::ViewContexts::UnsetCurrentContext()
{
    if (contextsStack.size() <= 1)
    {
        return false;
    }

    EndContext(currentContext);

    contextsStack.pop();

    return StartContext(contextsStack.top());
}


void Gex::Ui::ViewContexts::OnPressEvent(
        QMouseEvent* event
)
{
    if (!currentContextPtr)
    {
        return;
    }

    currentContextPtr->OnPressEvent(event);
}


void Gex::Ui::ViewContexts::OnMoveEvent(
        QMouseEvent* event
)
{
    if (!currentContextPtr)
    {
        return;
    }

    currentContextPtr->OnMoveEvent(event);
}


void Gex::Ui::ViewContexts::OnReleaseEvent(
        QMouseEvent* event
)
{
    if (!currentContextPtr)
    {
        return;
    }

    currentContextPtr->OnReleaseEvent(event);
}


bool Gex::Ui::ViewContexts::AcceptBaseEvents() const
{
    if (!currentContextPtr)
        return true;

    return currentContextPtr->AcceptBaseEvents();
}


void Gex::Ui::ViewContexts::RegisterContext(
        const std::string& ctx,
        ViewContext* ptr
)
{
    contexts[ctx] = ptr;
}


