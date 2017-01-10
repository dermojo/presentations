/**
 * @file	example.cpp
 * @author	daniel
 * @brief
 */

// Python - must be included first to avoid name clashes with Qt5 ("slots")
#include <Python.h>

// include Qt for a simple GUI
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>


// define a "CapturedStream" type and classes - these will replace sys.stdout and sys.stderr

class CapturedStreamImpl
{
public:
    CapturedStreamImpl(QPlainTextEdit& pe) : mEdit(pe) {}

    void write(const char* text, size_t n)
    {
        mBuffer.append(text, n);
        size_t pos;
        std::string line;
        while ((pos = mBuffer.find('\n')) != std::string::npos)
        {
            line = mBuffer.substr(0, pos + 1);
            mBuffer.erase(0, pos + 1);
            mEdit.appendPlainText(line.c_str());
        }
    }

private:
    QPlainTextEdit& mEdit;
    // used to buffer output until a newline is encountered
    std::string mBuffer;
};

// C-wrapper
struct CapturedStream
{
    PyObject_HEAD CapturedStreamImpl* cs;
};

static PyObject* CapturedStream_new(PyTypeObject* type, QPlainTextEdit& pe)
{
    CapturedStream* self = (CapturedStream*)type->tp_alloc(type, 0);
    if (self != NULL)
    {
        self->cs = new CapturedStreamImpl(pe);
    }
    return (PyObject*)self;
}

static void CapturedStream_dealloc(CapturedStream* self)
{
    delete self->cs;
    Py_TYPE(self)->tp_free((PyObject*)self);
}


static PyObject* CapturedStream_write(CapturedStream* self, PyObject* args)
{
    // the only expected argument is a string
    const char* s = NULL;
    if (!PyArg_ParseTuple(args, "s", &s))
        return NULL;

    if (s)
        self->cs->write(s, strlen(s));

    Py_RETURN_NONE;
}

static PyMethodDef CapturedStream_methods[] = {
    { "write", (PyCFunction)CapturedStream_write, METH_VARARGS, "Write to the stream" },
    { NULL } /* Sentinel */
};

static PyTypeObject example_CapturedStreamType = {
    PyVarObject_HEAD_INIT(NULL, 0) "CapturedStream", /* tp_name */
    sizeof(CapturedStream),                          /* tp_basicsize */
    0,                                               /* tp_itemsize */
    (destructor)CapturedStream_dealloc,              /* tp_dealloc */
    0,                                               /* tp_print */
    0,                                               /* tp_getattr */
    0,                                               /* tp_setattr */
    0,                                               /* tp_reserved */
    0,                                               /* tp_repr */
    0,                                               /* tp_as_number */
    0,                                               /* tp_as_sequence */
    0,                                               /* tp_as_mapping */
    0,                                               /* tp_hash  */
    0,                                               /* tp_call */
    0,                                               /* tp_str */
    0,                                               /* tp_getattro */
    0,                                               /* tp_setattro */
    0,                                               /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                              /* tp_flags */
    "A captured stream",                             /* tp_doc */
    0,                                               /* tp_traverse */
    0,                                               /* tp_clear */
    0,                                               /* tp_richcompare */
    0,                                               /* tp_weaklistoffset */
    0,                                               /* tp_iter */
    0,                                               /* tp_iternext */
    CapturedStream_methods,                          /* tp_methods */
    0,                                               /* tp_members */
    0,                                               /* tp_getset */
    0,                                               /* tp_base */
    0,                                               /* tp_dict */
    0,                                               /* tp_descr_get */
    0,                                               /* tp_descr_set */
    0,                                               /* tp_dictoffset */
    0,                                               /* tp_init */
    0,                                               /* tp_alloc */
    0,                                               /* tp_new */
    // CapturedStream_new,                              /* tp_new */
};

// contents of our "example" module
//
// static PyModuleDef example_module = { PyModuleDef_HEAD_INIT,
//                                      "example",
//                                      "Example module to demonstrate extending/embedding Python.",
//                                      -1,
//                                      NULL,
//                                      NULL,
//                                      NULL,
//                                      NULL,
//                                      NULL };
//
// PyMODINIT_FUNC PyInit_example(void)
//{
//    example_CapturedStreamType.tp_new = PyType_GenericNew;
//    if (PyType_Ready(&example_CapturedStreamType) < 0)
//        return NULL;
//
//    PyObject* m = PyModule_Create(&example_module);
//    if (m == NULL)
//        return NULL;
//
//    Py_INCREF(&example_CapturedStreamType);
//    PyModule_AddObject(m, "CapturedStream", (PyObject*)&example_CapturedStreamType);
//    return m;
//}

static bool initCapturedStream()
{
    example_CapturedStreamType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&example_CapturedStreamType) < 0)
        return false;

    Py_INCREF(&example_CapturedStreamType);
    return true;
}


class ExampleWindow : public QDialog
{
public:
    ExampleWindow() : QDialog()
    {
        QGridLayout* l = new QGridLayout(this);

        int row = 0;
        int col = 0;
        mButton1 = new QPushButton("Button 1", this);
        mButton1->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
        l->addWidget(mButton1, row, col);
        ++col;
        mButton2 = new QPushButton("Button 2", this);
        mButton2->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        l->addWidget(mButton2, row, col);
        ++col;
        mButton3 = new QPushButton("Button 3", this);
        mButton3->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
        l->addWidget(mButton3, row, col);
        ++col;
        mButton4 = new QPushButton("Button 4", this);
        mButton4->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
        l->addWidget(mButton4, row, col);

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

        setLayout(l);

        initScripting();
    }

    ~ExampleWindow() { cleanupScripting(); }

private:
    void initScripting()
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

                PyObject* myStdout = CapturedStream_new(&example_CapturedStreamType, *mLogWindow);
                Py_INCREF(myStdout);
                PyObject_SetAttrString(sys, "stdout", myStdout);
                PyObject_SetAttrString(sys, "stderr", myStdout);
                //                Py_DECREF(myStdout);

                Py_DECREF(sys);
            }
        }
    }

    void cleanupScripting()
    {
        // tear down the interpreter
        Py_Finalize();
    }

    void executeScript()
    {
        // get the text from the input field
        QString text = mScriptInput->toPlainText();
        int rc = PyRun_SimpleString(text.toStdString().c_str());

        // TODO: print exception on error
        (void)rc;
    }

    void logLine(const QString& line)
    {
        mLogWindow->appendPlainText(line);
        if (!line.endsWith('\n'))
            mLogWindow->appendPlainText("\n");
    }

    void makeNice(QPlainTextEdit* pe)
    {
        // set some general formatting options
        pe->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    }

private:
    QPushButton* mButton1;
    QPushButton* mButton2;
    QPushButton* mButton3;
    QPushButton* mButton4;
    QPlainTextEdit* mScriptInput;
    QPlainTextEdit* mLogWindow;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    ExampleWindow win;
    win.show();

    return app.exec();
}
