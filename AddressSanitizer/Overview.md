Address Sanitizer
=================

About
-----
* AddressSanitizer (aka ASan) is a memory error detector for C/C++
* Developed by Google (see: [GitHub](https://github.com/google/sanitizers))

It finds
--------
* Use after free (dangling pointer dereference)
* Heap buffer overflow
* Stack buffer overflow
* Global buffer overflow
* Use after return
* Use after scope
* Initialization order bugs
* Memory leaks (it includes the "Leak Sanitizer")

Components
----------
* Consists of a compiler instrumentation module and a run-time library (replacing malloc/free)
* Very fast: average slowdown of the instrumented program is ~2x

Supported platforms
-------------------
* Currently implemented in:
    * Clang (3.1+)
    * GCC (4.8+)
    * Xcode (7.0+)
* works on x86, ARM, MIPS (both 32- and 64-bit versions of all architectures), PowerPC64
* supported OSes: Linux, Darwin (OS X and iOS Simulator), FreeBSD, Android
* Windows support only for C and fragile ...

Examples
--------
... just compile with `-fsanitize=address`
