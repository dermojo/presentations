Thread Sanitizer
================

About
-----
* ThreadSanitizer (aka TSan) is a data race detector for C/C++
* Developed by Google (see: [GitHub](https://github.com/google/sanitizers))

It finds
--------
* Normal data races
* Races on C++ object vptr
* Use after free races
* Races on mutexes, file descriptors, barriers
* Destruction of a locked mutex
* Leaked threads
* Signal-unsafe malloc/free calls in signal handlers
* Signal handler spoils errno
* Potential deadlocks (lock order inversions)

Supported platforms
-------------------
* TSan is supported on:
    * Linux: x86_64, mips64 (40-bit VMA), aarch64 (39/42-bit VMA), powerpc64 (44/46/47-bit VMA)
    * Mac: x86_64, aarch64 (39-bit VMA)
    * FreeBSD: x86_64
    * NetBSD: x86_64
* Currently implemented in:
    * Clang (3.2+)
    * GCC (4.8+)

Examples
--------
... just compile and link with `-fsanitize=thread`
(optimizations and debug information recommended)
