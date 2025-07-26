# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I./include -I/opt/homebrew/Cellar/boost/1.88.0/include/

# Directories
SRCDIR = src
INCDIR = include
BUILDDIR = build

# Source and object files
SOURCES := test.cpp
OBJECTS := $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SOURCES))

# Output binary
TARGET = test

# Default target
all: $(TARGET)

# Link all object files into final binary
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp to .o
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Clean up build artifacts
clean:
	rm -rf $(BUILDDIR) $(TARGET)
