/**
 * @file    captured_stream.hpp
 * @author  daniel
 * @brief   Defines the "Captured Stream" Python class
 *
 * Explicit use of the Python C API for a specific reason:
 * "CapturedStream" instances can only be created from inside the C++ application. Python code
 * should not (and cannot) create them because they need a QPlainTextEdit instance.
 */

#ifndef CAPTURED_STREAM_HPP_
#define CAPTURED_STREAM_HPP_

// Python - must be included first to avoid name clashes with Qt5 ("slots")
#include <Python.h>

#include <QtWidgets/QPlainTextEdit>

// define a "CapturedStream" type and classes - these will replace sys.stdout and sys.stderr

bool initCapturedStream();

PyObject* createCapturedStream(QPlainTextEdit& pe);

#endif /* CAPTURED_STREAM_HPP_ */
