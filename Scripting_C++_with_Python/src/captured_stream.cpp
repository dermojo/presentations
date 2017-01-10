/**
 * @file    captured_stream.cpp
 * @author  daniel
 * @brief
 */

#include "captured_stream.hpp"

#include <QtWidgets/QApplication>

/**
 * This is the C++ implementation of the CapturedStream class, which logs output into a
 * QPlainTextEdit.
 */
class CapturedStreamImpl
{
public:
    CapturedStreamImpl(QPlainTextEdit& pe) : mEdit(pe) {}

    void write(const char* text, size_t n);

private:
    QPlainTextEdit& mEdit;
    // used to buffer output until a newline is encountered
    std::string mBuffer;
};


void CapturedStreamImpl::write(const char* text, size_t n)
{
    mBuffer.append(text, n);
    size_t pos;
    std::string line;
    while ((pos = mBuffer.find('\n')) != std::string::npos)
    {
        line = mBuffer.substr(0, pos + 1);
        mBuffer.erase(0, pos + 1);
        mEdit.appendPlainText(line.c_str());
        // for a UI update
        qApp->processEvents();
    }
}

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

bool initCapturedStream()
{
    example_CapturedStreamType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&example_CapturedStreamType) < 0)
        return false;

    Py_INCREF(&example_CapturedStreamType);
    return true;
}

PyObject* createCapturedStream(QPlainTextEdit& pe)
{
    return CapturedStream_new(&example_CapturedStreamType, pe);
}
