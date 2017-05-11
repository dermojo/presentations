.PHONY: all clean

CXX := g++
CXXFLAGS := -std=c++11 -Wall
PYTHON_CFLAGS = $(shell pkg-config --cflags python3)
LDFLAGS := -lcrypto
PYTHON_LDFLAGS := $(shell pkg-config --libs python3)
EXECUTABLES := mypy sign

all: $(EXECUTABLES)

clean:
	rm -f $(EXECUTABLES) *.o

mypy: main.cpp accesscontrol.cpp signatures.cpp
	$(CXX) $(CXXFLAGS) $(PYTHON_CFLAGS) -o $@ $^ $(LDFLAGS) $(PYTHON_LDFLAGS)

sign: sign_main.cpp signatures.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# generate a key pair with OpenSSL:
# $ openssl genrsa -out privkey.pem
# $ openssl rsa -in privkey.pem -pubout -out pubkey.pem

# To sign a script (with a detached signature):
# $ ./sign foo.py

# To create a "stand-alone" executable:
# $ cp foo.py __main__.py
# $ zip foo.zip __main__.py
# $ ./sign --iszip foo.zip
# $ mv foo.zip.standalone foo
# $ ./foo
