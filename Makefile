CXXFLAGS = -std=c++11 -Ofast -Wall -Werror
LDFLAGS = -lSDL2
OS = $(shell uname -s)
SRC = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SRC))
HEADERS = $(patsubst %.cpp, %.h, $(SRC))
DEPS = $(patsubst %.cpp, %.d, $(SRC))
ELFNAME = cliptest

ifeq ($(OS), Darwin)
	CXX = g++-4.9
endif
ifeq ($(OS), Linux)
	CXX = g++
endif

all: screen.so screen.a $(ELFNAME)

$(ELFNAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o$@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -MMD -MP $< -o $@

screen.so: Screen.cpp Screen.h Color.o Color.h
	$(CXX) $(CXXFLAGS) -shared -fPIC $(LDFLAGS) -o$@ $^

screen.a: Screen.o
	ar -cvq $@ $^

-include $(DEPS)

clean:
	rm -f *.d screen.so screen.a *.o $(ELFNAME)
