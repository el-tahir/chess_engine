## Simple Makefile for Linux and Windows (MinGW)

CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra

SRCS := main.cpp board.cpp
OBJS := $(SRCS:.cpp=.o)

TARGET := chess_engine

ifeq ($(OS),Windows_NT)
  EXE := .exe
  RM := del /Q
else
  EXE :=
  RM := rm -f
endif

all: $(TARGET)$(EXE)

$(TARGET)$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(TARGET)$(EXE)

win:
	make CXX=x86_64-w64-mingw32-g++ TARGET=chess_engine EXE=.exe
