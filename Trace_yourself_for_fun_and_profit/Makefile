.PHONY: all clean

CXX := g++
CXXFLAGS := -std=c++11 -O2 -Wall -Werror
LDFLAGS := -lcrypto

EXECUTABLES := secret_application secret_application_dbgcheck secret_application_trace

all: $(EXECUTABLES)

secret_application: secret_application.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

secret_application_dbgcheck: secret_application.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) -DCHECK_FOR_DEBUGGER

secret_application_trace: secret_application.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) -DPTRACE_MYSELF

clean:
	rm -f $(EXECUTABLES) *.o
