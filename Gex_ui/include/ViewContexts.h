#ifndef GEX_VIEWCONTEXTS_H
#define GEX_VIEWCONTEXTS_H

#include "api.h"
#include <stack>
#include <QMouseEvent>
#include <QMouseEvent>

namespace Gex::Ui
{
    class BaseGraphView;


    class GEX_UI_API ViewContext
    {
        BaseGraphView* currentView = nullptr;

    public:
        virtual ~ViewContext() = default;

        virtual bool AcceptBaseEvents() const = 0;

        BaseGraphView* CurrentView() const;

        void Activate(BaseGraphView* scene);

        virtual void Finalize(BaseGraphView* scene);

        virtual void Setup(BaseGraphView* scene) = 0;

        virtual void OnPressEvent(QMouseEvent* event) = 0;

        virtual void OnMoveEvent(QMouseEvent* event) = 0;

        virtual void OnReleaseEvent(QMouseEvent* event) = 0;
    };


    class GEX_UI_API ViewContexts
    {
        BaseGraphView* currentView;
        std::map<std::string, ViewContext*> contexts;
        std::string currentContext;
        ViewContext* currentContextPtr = nullptr;

        std::stack<std::string> contextsStack;

    public:
        ViewContexts(BaseGraphView* scene);

        ~ViewContexts();

        std::string CurrentContext() const;

        bool StartContext(const std::string& context);

        bool EndContext(const std::string& context);

        bool SetCurrentContext(const std::string& context);

        bool UnsetCurrentContext();

        void OnPressEvent(QMouseEvent* event);

        void OnMoveEvent(QMouseEvent* event);

        void OnReleaseEvent(QMouseEvent* event);

        bool AcceptBaseEvents() const;

        void RegisterContext(const std::string& ctx, ViewContext* ptr);

        template<class Ctx>
        void RegisterContext(const std::string& ctx)
        {
            RegisterContext(ctx, new Ctx());
        }
    };
}

#endif //GEX_VIEWCONTEXTS_H
