UB Sanitizer
============

About
-----
* UndefinedBehaviorSanitizer (aka UBSan) is a fast undefined behavior detector
* As usual: compile time instrumentation + runtime library

It finds
--------
* Pointer/reference alignment checks
* Loading a bool that isn't true/false (0/1)
* Passing invalid values to compiler builtins
* Out of bounds array indexing
* Loading an invalid enum
* Floating-point conversion overflows + division by 0
* Integer division by zero
* Using null with non-null-parameters/return values/...
* Use of a null pointer or creation of a null reference


It finds
--------
* Object out-of-bounds access (optimizations!)
* Pointer arithmetic overflows
* Return without value from non-void functions
* Shifting with bad exponent, shift overflow
* Signed integer overflow (taking promotion into account)
* Reaching an unreachable code point
* VLA bounds that aren't positive
* Object usage & conversions with incorrect object types (requires RTTI)


Clang only (not UB, but usually unintended):
--------------------------------------------
* Call through mismatching function pointer type
* Implicit integer conversion with data loss or sign change
* Unsigned integer overflow


Supported platforms
-------------------
* GCC & Clang
* According to Clang:
    * Android
    * Linux
    * NetBSD
    * FreeBSD
    * OpenBSD
    * macOS
    * Windows


Examples
--------
... just compile and link with `-fsanitize=undefined`

(specific/optional checks can be selected as well)
