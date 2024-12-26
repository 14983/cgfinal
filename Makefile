CXX      := g++
CXXFLAGS := -Wall -Iinclude

MODULES_SRC := $(wildcard modules/*.cpp)
MODULES_OBJ := $(MODULES_SRC:.cpp=.o)

CURRENT_SRC := $(wildcard *.cpp)
CURRENT_OBJ := $(CURRENT_SRC:.cpp=.o)

ifeq ($(OS),Windows_NT)
	LDFLAGS  := -L ./depends -lopengl32 -lfreetype -lgdi32
	GLFW_LIB := depends/libglfw3.a
	CLEAN_CMD := del
	CLEAN_OPTS := /Q
	TARGET := main.exe
	CLEAN_OBJ := modules\*.o *.o $(TARGET) depends\glad.o
else
	LDFLAGS  := -lGL -lGLU -lglfw -lGLEW -lm -lstb
	GLFW_LIB :=
	CLEAN_CMD := rm
	CLEAN_OPTS := -f
	TARGET := main
	CLEAN_OBJ := modules/*.o *.o $(TARGET) depends/glad.o
endif

all: $(TARGET)

$(TARGET): $(MODULES_OBJ) $(CURRENT_OBJ) depends/glad.o $(GLFW_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

depends/glad.o: depends/glad.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(CLEAN_CMD) $(CLEAN_OPTS) $(CLEAN_OBJ)

.PHONY: all clean
