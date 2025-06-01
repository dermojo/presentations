# Advanced unit testing and mocking

## Advanced topics

- when using functions or loops (DRY)...
    - ASSERT_x in non-void functions
    - custom messages
    - scopes
- parametrized tests (instead of loops)
    - (type parameters)

- when using custom classes...
    - custom printers
    - predicate assertions (e.g. `EXPECT_PRED2(jsonIsEqual, a, b)`)
    - create a custom ASSERT/EXPECT macro (e.g. std::expected)

- exceptions, floats
- repetitions & filters
- death tests

## Advanced mocking

- advanced mock expectations
- default actions
- mocking functions instead of interfaces
    - mocking C functions (__wrap)
- matchers
- factories & moving mocks
- default parameters
- handling overloads
- mocks & fork
    - set up the mock in the process where it's used
    - use a factory to create it on demand

## Best practices

- What makes a good test?
- Size vs overhead
- keep helper functions/classes in separate files/TUs
- keep mocks in separate files/headers
- use specific macros (e.g. _EQ), avoid ASSERT_TRUE/FALSE
- use custom predicates/macros for custom error codes/objects

TODO: Coverage (gcov, OpenCppCoverage)
