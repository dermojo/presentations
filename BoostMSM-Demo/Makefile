CXX := g++ -pipe
CXXFLAGS := -Wall -I def -std=c++0x -pthread
LIBS :=	-lboost_system
VPATH := src

all: doors

clean:
	rm -f doors *.o
	
%.o: %.cc
	$(CXX) -c -o $@ $^ $(CXXFLAGS)

doors: doors.o main.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)
