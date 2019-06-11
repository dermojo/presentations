#CXX := g++-8
CXX := clang++-6.0
CXXFLAGS := -O2 -fsanitize=thread -pipe -g -Wall -std=c++14
LDFLAGS :=

all: 01_race.exe \
	 02_race_vptr.exe \
	 03_race_on_free.exe \
	 04_destroy_locked_mutex.exe

clean:
	rm -f *.exe *.o

%.exe: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
