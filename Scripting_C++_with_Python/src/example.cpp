/**
 * @file    example.cpp
 * @author	daniel
 * @brief   Exmaple main program - QT GUI with script capabilities (4 buttons + 4 icons)
 */

#include <Python.h>
#include "pybind11/pybind11.h"
#include "pybind11/functional.h"

#include <memory>
#include <vector>
#include <string>

// include Qt for a simple GUI
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>

#include "captured_stream.hpp"

namespace py = pybind11;


/**
 * Encapulates access to the Python interpreter.
 */
class PythonInterpreter
{
public:
    PythonInterpreter(QPlainTextEdit& peLog) : mLogEdit(peLog)
    {
        if (!Py_IsInitialized())
        {
            wchar_t progname[] = L"example";
            Py_SetProgramName(progname); /* optional but recommended */
            // must be called before doing anything else
            Py_Initialize();

            // print the interpreter version
            const char* pyver = Py_GetVersion();
            QString line = "Python ";
            line += pyver;
            line += "\n";
            logLine(line);

            // register our built-in types
            if (!initCapturedStream())
            {
                logLine("Failed to initialize CapturedStream!\n");
                return;
            }

            // redirect sys.stdout and sys.stderr to our log window
            // 1. import the 'sys' module
            PyObject* sys = PyImport_ImportModule("sys");
            if (!sys)
                logLine("Interpreter broken - 'sys' not found!");
            else
            {
                // all Python objects are reference counted
                Py_INCREF(sys);

                // 2. create an instance of the stream capture class
                PyObject* myStdout = createCapturedStream(mLogEdit);
                Py_INCREF(myStdout);
                // 3. set sys.stdout / sys.stderr
                PyObject_SetAttrString(sys, "stdout", myStdout);
                PyObject_SetAttrString(sys, "stderr", myStdout);
                // Py_DECREF(myStdout);

                Py_DECREF(sys);
            }

            // create a dictionary for global variables
            mGlobals = PyDict_New();
            PyDict_SetItemString(mGlobals, "__builtins__", PyEval_GetBuiltins());
        }
    }
    ~PythonInterpreter()
    {
        // tear down the interpreter
        Py_Finalize();
    }

    void execute(const std::string& cmds)
    {
#if 0
        // this is a very simple, but limited way to execute the script...
        PyRun_SimpleString(cmds.c_str());
#else
        PyObject* res = PyRun_StringFlags(cmds.c_str(), Py_file_input, mGlobals, mGlobals, nullptr);
        if (res)
        {
            if (res != Py_None)
            {
                // there was a result: print repr(res)
                PyObject* repr = PyObject_Repr(res);

                // no error handling here...
                PyObject* bytes = PyUnicode_AsUTF8String(repr);
                std::string reprString = PyBytes_AsString(bytes);
                Py_DECREF(bytes);

                std::string line = "result: ";
                line += reprString;
                logLine(line.c_str());

                Py_DECREF(repr);
            }
            Py_DECREF(res);
        }
        else
        {
            // exception!
            PyObject *errtype, *errvalue, *traceback;
            PyErr_Fetch(&errtype, &errvalue, &traceback);
            if (errvalue != NULL)
            {
                PyObject* s = PyObject_Str(errvalue);

                PyObject* bytes = PyUnicode_AsUTF8String(s);
                std::string reprString = PyBytes_AsString(bytes);
                Py_DECREF(bytes);

                std::string line = "EXCEPTION: ";
                line += reprString;
                logLine(line.c_str());

                Py_DECREF(s);
            }
            Py_XDECREF(errvalue);
            Py_XDECREF(errtype);
            Py_XDECREF(traceback);
        }
#endif
    }

private:
    void logLine(const QString& line)
    {
        mLogEdit.appendPlainText(line);
        if (!line.endsWith('\n'))
            mLogEdit.appendPlainText("\n");
        // force a UI update
        qApp->processEvents();
    }

private:
    QPlainTextEdit& mLogEdit;
    PyObject* mGlobals;
};

/**
 * UI component - shows interpreter output, provides input to the interpreter and has buttons
 * that can be manipulated.
 */
class ExampleWindow : public QDialog
{
public:
    ExampleWindow() : QDialog()
    {
        QGridLayout* l = new QGridLayout(this);

        int row = 0;
        int col = 0;
        for (int i = 0; i < 4; ++i)
        {
            std::string name = "Button ";
            name += std::to_string(i + 1);
            mButtons[i] = new QPushButton(name.c_str(), this);
            //            mButtons[i]->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
            l->addWidget(mButtons[i], row, col);
            connect(mButtons[i], &QPushButton::clicked, [this, i]() { onButtonClick(i); });
            ++col;
        }

        ++row;
        col = 0;

        for (int i = 0; i < 4; ++i)
        {
            mIcons[i] = new QLabel(this);
            mIcons[i]->setPixmap(QPixmap(":/myresources/online-icon.png"));
            l->addWidget(mIcons[i], row, col);
            ++col;
        }

        ++row;
        col = 0;

        l->addWidget(new QLabel("Enter the Python code to execute below:", this), row, col, 1, 4);
        ++row;

        mScriptInput = new QPlainTextEdit("# insert commands here\n", this);
        makeNice(mScriptInput);
        l->addWidget(mScriptInput, row, col, 4, 4);
        row += 4;

        QPushButton* execScript = new QPushButton("Execute script", this);
        l->addWidget(execScript, row, col);
        connect(execScript, &QPushButton::clicked, [this]() { executeScript(); });
        ++row;

        l->addWidget(new QLabel("Interpreter output:", this), row, col, 1, 4);
        ++row;

        mLogWindow = new QPlainTextEdit();
        makeNice(mLogWindow);
        mLogWindow->setReadOnly(true);
        l->addWidget(mLogWindow, row, col, 2, 4);
        row += 2;

        ++row;
        col = 0;

        mQuitButton = new QPushButton("Quit", this);
        l->addWidget(mQuitButton, row, col);
        connect(mQuitButton, &QPushButton::clicked, [this]() { close(); });

        setLayout(l);

        initScripting();
    }


    void addButtonCallback(std::function<void(int)> cb)
    {
        mButtonCallbacks.emplace_back(std::move(cb));
    }
    void clearCallbacks() { mButtonCallbacks.clear(); }
    bool setIconState(int num, bool on)
    {
        if (num >= 0 && num < 4)
        {
            if (on)
                mIcons[num]->setPixmap(QPixmap(":/myresources/online-icon.png"));
            else
                mIcons[num]->setPixmap(QPixmap(":/myresources/invisible-icon.png"));

            // for a UI update
            qApp->processEvents();
            return true;
        }
        // bad parameter
        return false;
    }

private:
    void initScripting()
    {
        if (!mInterpreter)
        {
            mInterpreter.reset(new PythonInterpreter(*mLogWindow));

            // create a internal module that provides access to some functions
            py::module m("example", "My example module with pybind11");
            m.def("addCallback", [this](std::function<void(int)> cb) { addButtonCallback(cb); });
            m.def("clearCallbacks", [this]() { clearCallbacks(); });
            m.def("setIconState",
                  [this](int num, bool on) -> bool { return setIconState(num, on); },
                  py::arg("icon"), py::arg("on") = true);

            // manual register the module - it's a built-in module so the standard ways don't work
            auto sys = py::module::import("sys");
            sys.attr("modules")["example"] = m;
        }
    }

    void executeScript()
    {
        // get the text from the input field
        QString text = mScriptInput->toPlainText();
        mInterpreter->execute(text.toStdString());
    }

    void makeNice(QPlainTextEdit* pe)
    {
        // set some general formatting options
        pe->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    }

    void onButtonClick(int i)
    {
        for (auto& cb : mButtonCallbacks)
            cb(i);
    }

private:
    QPushButton* mButtons[4];
    QLabel* mIcons[4];
    QPlainTextEdit* mScriptInput;
    QPlainTextEdit* mLogWindow;
    QPushButton* mQuitButton;

    std::unique_ptr<PythonInterpreter> mInterpreter;

    std::vector<std::function<void(int)>> mButtonCallbacks;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(example);

    ExampleWindow win;
    win.show();

    return app.exec();
}
