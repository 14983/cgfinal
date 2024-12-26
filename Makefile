CXX      := g++
CXXFLAGS := -Wall -Iinclude

ifeq ($(OS),Windows_NT)
	LDFLAGS  := -lopengl32 -lglu32 -lglfw3 -lglew32 -lm
else
	LDFLAGS  := -lGL -lGLU -lglfw -lGLEW -lm -lstb
endif

MODULES_SRC := $(wildcard modules/*.cpp)
MODULES_OBJ := $(MODULES_SRC:.cpp=.o)

CURRENT_SRC := $(wildcard *.cpp)
CURRENT_OBJ := $(CURRENT_SRC:.cpp=.o)

TARGET = main

all: $(TARGET)

$(TARGET): $(MODULES_OBJ) $(CURRENT_OBJ) depends/glad.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

depends/glad.o: depends/glad.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(MODULES_OBJ) $(CURRENT_OBJ) $(TARGET) depends/glad.o

.PHONY: all clean