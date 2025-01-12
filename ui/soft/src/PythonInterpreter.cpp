#include "ui/soft/include/PythonInterpreter.h"

#include <regex>
#include <iostream>

#include <Qt>
#include <QBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QSplitter>
#include <QShortcut>
#include <QTimer>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

#include "ui/soft/include/PythonOutput.h"




QStringList keywords = {"and", "as", "assert", "break", "class", "continue", "def",
                        "del", "elif", "else", "except", "False", "finally", "for",
                        "from", "global", "if", "import", "in", "is", "lambda",
                        "None", "nonlocal", "not", "or", "pass", "raise", "return",
                        "True", "try", "while", "with", "yield"};


void GexSoftware::Highlighter::DoMatch(const QString& exp, const QString& text,
                                       QTextCharFormat& formatter)
{
    QRegularExpression expr(exp);
    QRegularExpressionMatchIterator iter = expr.globalMatch(text);
    while (iter.hasNext())
    {
        QRegularExpressionMatch match = iter.next();
        setFormat(match.capturedStart(),
                  match.capturedLength(),
                  formatter);
    }
}


void GexSoftware::Highlighter::highlightBlock(const QString &text)
{
    QTextCharFormat kwformatter;
    kwformatter.setFontWeight(QFont::Bold);
    kwformatter.setForeground(Qt::darkGreen);

    DoMatch("\\b(" + keywords.join("|") + ")\\b", text, kwformatter);

    QTextCharFormat strformatter;
    strformatter.setForeground(Qt::yellow);

    DoMatch("(\")[^\"]*(\")", text, strformatter);

    DoMatch("(')[^']*(')", text, strformatter);

    QTextCharFormat numberformatter;
    numberformatter.setForeground(Qt::darkCyan);

    DoMatch("\\b[0-9.]*\\b", text, numberformatter);

    QTextCharFormat commentformatter;
    commentformatter.setForeground(Qt::darkGray);

    DoMatch("#[^\n]*", text, commentformatter);
}


GexSoftware::PythonInterpreter::PythonInterpreter(QWidget* parent): QDockWidget(parent)
{
    refreshTimer = new QTimer(this);
    refreshTimer->setSingleShot(true);

    setWindowTitle("Python console");

    QSplitter* layout = new QSplitter(this);
    layout->setOrientation(Qt::Vertical);
    setWidget(layout);

    read = new QTextEdit(this);
    read->setReadOnly(true);
    layout->addWidget(read);

    QWidget* downFrame = new QWidget(this);
    layout->addWidget(downFrame);

    QHBoxLayout* downLayout = new QHBoxLayout();
    downLayout->setContentsMargins(0, 0, 0, 0);
    downLayout->setSpacing(0);
    downFrame->setLayout(downLayout);

    write = new QTextEdit(this);
    downLayout->addWidget(write);

    highlighter = new Highlighter(write->document());

    QVBoxLayout* buttonLayout = new QVBoxLayout();
    buttonLayout->setContentsMargins(5, 5, 5, 5);
    buttonLayout->setSpacing(5);
    buttonLayout->setAlignment(Qt::AlignTop);
    downLayout->addLayout(buttonLayout);

    QPushButton* playButton = new QPushButton(this);
    playButton->setFixedSize(20, 20);
    playButton->setText("Run");
    buttonLayout->addWidget(playButton);

    QPushButton* clearButton = new QPushButton(this);
    clearButton->setFixedSize(20, 20);
    clearButton->setText("Clear");
    buttonLayout->addWidget(clearButton);

    locals = PyDict_New();
    globals = PyDict_New();

    QObject::connect(playButton, &QPushButton::clicked,
                     this, &PythonInterpreter::RunCurrentEditorCode,
                     Qt::QueuedConnection);
    QObject::connect(clearButton, &QPushButton::clicked,
                     write, &QTextEdit::clear,
                     Qt::QueuedConnection);

    QShortcut* enter = new QShortcut(QKeySequence(Qt::Key_Control | Qt::Key_Enter), this);
    enter->setContext(Qt::WindowShortcut);
    QObject::connect(enter, &QShortcut::activated, this,
                     &PythonInterpreter::RunCurrentEditorCode,
                     Qt::QueuedConnection);
}


void GexSoftware::PythonInterpreter::Write(std::string str)
{
    read->setReadOnly(false);

    read->setPlainText(read->toPlainText() + str.c_str());

    read->setReadOnly(true);

    read->verticalScrollBar()->setValue(read->verticalScrollBar()->maximum());
}


void GexSoftware::PythonInterpreter::RunCurrentEditorCode()
{
    QString rawcode = write->toPlainText();

    Write(rawcode.toStdString() + "\n");

    PyObject* result = PyRun_String(rawcode.toStdString().c_str(), Py_file_input,
                                    globals, locals);
    if (PyErr_Occurred())
    {
        PyErr_Print();
    }

    else if (result != Py_None)
    {
        PyObject* repr = PyObject_Repr(result);
        std::string s = PyUnicode_AsUTF8(repr);
        Write("# result : " + s);
    }
}


void GexSoftware::PythonInterpreter::Initialize()
{
    int res = PyRun_SimpleString("import SoftApi\n"
                                 "import sys\n"
                                 "sys.stdout = SoftApi.PythonOutput\n"
                                 "sys.stderr = SoftApi.PythonOutput"
                                 );
    if (res)
    {
        PyErr_Print();
    }

    auto output = SoftPython::PythonOutput::GetInstance();

    output->RegisterCallback([this](std::string msg){this->Write(msg);});
}
