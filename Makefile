CXX = g++
CXXFLAGS = -pedantic -Wall -Werror 
CXXFLAGS_DEBUG := -Wno-unused-function -Wno-unused-variable
LIBS = -lSDL2 -lGLEW -lGL -lGLU -lm

vpath %.c src $(wildcard src/*/.)
vpath %.o build

SOURCEDIR = src
BUILDDIR = build

IGNORE := 
SOURCES := $(filter-out $(IGNORE), $(notdir $(wildcard $(SOURCEDIR)/*.c $(SOURCEDIR)/*/*.c)) )
OBJECTS := $(SOURCES:.c=.o)

TARGET = app


all: $(TARGET)

release : FORCE disable_debug all

disable_debug :
CXXFLAGS_DEBUG := 

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_DEBUG) $(addprefix $(BUILDDIR)/, $(OBJECTS)) -o $(BUILDDIR)/$@ $(LIBS)
	ln -fs build/app app


$(OBJECTS): %.o: %.c 
	$(CXX) -c $(CXXFLAGS) $< -o $(BUILDDIR)/$@ 


clean: FORCE
	rm -rf $(addprefix $(BUILDDIR)/, $(TARGET) $(OBJECTS))


remake: clean all 


FORCE: