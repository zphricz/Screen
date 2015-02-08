CXXFLAGS = -std=c++11 -Ofast -Wall -Werror
LDFLAGS = -lSDL2
OS= $(shell uname -s)

ifeq ($(OS), Darwin)
	CXX = g++-4.9
endif
ifeq ($(OS), Linux)
	CXX = g++
endif

all: screen.so screen.a

screen.so: Screen.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC $(LDFLAGS) -o$@ $^

screen.a: Screen.o
	ar -cvq $@ $^

clean:
	rm -f screen.so screen.a *.o
