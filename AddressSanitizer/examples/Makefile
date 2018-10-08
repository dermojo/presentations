#CXX := g++
CXX := clang++-6.0
CXXFLAGS := -O2 -fsanitize=address -pipe -g -Wall -std=c++14
LDFLAGS :=

all: 01_heap_use_after_free.exe \
	 02_heap_buffer_overflow.exe \
	 03_stack_buffer_overflow.exe \
	 04_global_buffer_overflow.exe \
	 05_use_after_scope.exe \
	 06_leak.exe \
	 07_use_after_return.exe

clean:
	rm -f *.exe *.o

%.exe: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
