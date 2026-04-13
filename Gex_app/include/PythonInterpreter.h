#ifndef NODELIBRARY2COMPLETE_PYTHONINTERPRETER_H
#define NODELIBRARY2COMPLETE_PYTHONINTERPRETER_H

#include <QDockWidget>
#include <QWidget>
#include <QTextEdit>
#include <QTimer>
#include <QSyntaxHighlighter>

#include "pybind11/pybind11.h"
#include "softApi.h"

#include "Gex/include/Interpreter.h"

namespace GexApp
{
    class Highlighter: public QSyntaxHighlighter
    {
    public:
        using QSyntaxHighlighter::QSyntaxHighlighter;

        void DoMatch(const QString& exp, const QString& text,
                     QTextCharFormat& formatter);

        void highlightBlock(const QString &text) override;
    };

    class PythonInterpreter;

    class PyOutputCallback: public QObject
    {
    Q_OBJECT
        PythonInterpreter* edit;

    public:
        PyOutputCallback(GexApp::PythonInterpreter* interpreter,
                         QObject* parent=nullptr);

//        void Exec(const std::string& output) const;
        Q_SIGNAL void SendEvent(const std::string& output);

        void ReceiveEvent(const std::string& output);
    };

    class SOFT_API PythonInterpreter: public QDockWidget
    {
        QTextEdit* read;
        QTextEdit* write;
        QTimer* refreshTimer;
        PyObject* locals;
        PyObject* globals;
        Highlighter* highlighter;
        static bool registered;

        Gex::Python::InterpreterHandlePtr handle;

    public:
        PythonInterpreter(QWidget* parent=nullptr);

    public:
        void RunCurrentEditorCode();

        void Write(std::string str);

        void Initialize();
    };
}

#endif //NODELIBRARY2COMPLETE_PYTHONINTERPRETER_H
