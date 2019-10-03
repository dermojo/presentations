#CXX := g++-8
CXX := clang++-6.0
CXXFLAGS := -O2 -fsanitize=undefined -pipe -g -Wall -std=c++14
LDFLAGS :=

all: 01_pointer_alignment.exe \
	 02_invalid_bool.exe \
	 03_out_of_bounds.exe \
	 04_invalid_enum.exe \
	 05_type_violation.exe \
	 06_null_pointer.exe \
	 07_invalid_object_size.exe

clean:
	rm -f *.exe *.o

%.exe: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
