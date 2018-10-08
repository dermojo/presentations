#CXX := g++
CXX := clang++-6.0
CXXFLAGS := -O2 -fsanitize=address -pipe -g -Wall -Werror -std=c++14
LDFLAGS :=

all: 01_heap_use_after_free.exe \
	 02_heap_buffer_overflow.exe \
	 03_stack_buffer_overflow.exe \
	 04_use_after_return.exe \
	 05_use_after_scope.exe \
	 06_leak.exe

clean:
	rm -f *.exe *.o

%.exe: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
