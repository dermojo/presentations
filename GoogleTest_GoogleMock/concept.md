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
- VS code integration

# How to use GMock

- CMake setup
- simple example
- strict vs. nice mock
- setting expectations

## Best practices

- start with strict mocks
- mocking classes from external libs
