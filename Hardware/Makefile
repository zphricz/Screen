CXXFLAGS = -std=c++11 -Ofast -Wall -Werror
LDFLAGS = -lSDL2
OS = $(shell uname -s)
SRC = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SRC))
HEADERS = $(patsubst %.cpp, %.h, $(SRC))
DEPS = $(patsubst %.cpp, %.d, $(SRC))
ELFNAME = clip_test_hardware

ifeq ($(OS), Darwin)
	CXX = clang++
endif
ifeq ($(OS), Linux)
	CXX = g++
endif

all: hardware_screen.so hardware_screen.a $(ELFNAME)

$(ELFNAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o$@ $^ $(LDFLAGS) 

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -MMD -MP $< -o $@

hardware_screen.so: HardwareScreen.cpp HardwareScreen.h Color.cpp Color.h
	$(CXX) $(CXXFLAGS) -shared -fPIC -o$@ $< $(LDFLAGS)

hardware_screen.a: HardwareScreen.o
	ar -cvq $@ $^

-include $(DEPS)

clean:
	rm -f *.d hardware_screen.so hardware_screen.a *.o $(ELFNAME)
