# Why unit tests?

- make sure your code works
- manual tests are expensive
- feel confident in your code :)
- greatest combo: unit tests + sanitizers
- aim for high coverage

# How to use GTest

- CMake setup(?)
- assert vs. expect
- test fixtures, setup, teardown
    - deriving from other fixtures - remember to call the base class' SetUp()/TearDown()
- custom messages
- scopes
- custom printers
- VS code integration

## Advanced topics

- exceptions, predicate assertions, floats
- parametrized tests (instead of loops)
- type parameters
- ASSERT_x in non-void functions
- repetitions
- filters
- debugging tests
- death tests

## Best practices

- What makes a good test?
- Size vs overhead
- keep helper functions/classes in separate files/TUs
- use specific macros (e.g. _EQ), avoid ASSERT_TRUE/FALSE
- use custom macros for custom error codes/objects

# How to use GMock

- CMake setup
- simple example
- strict vs. nice mock
- setting expectations
- advanced expectations

## Advanced topics

- default actions
- mocking functions instead of interfaces
- matchers
- factories & moving mocks
- default parameters
- handling overloads

## Best practices

- start with strict mocks
- mocking classes from external libs
- mocks & fork
- keep mocks in separate files/headers
