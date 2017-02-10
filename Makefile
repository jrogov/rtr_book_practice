CPP = g++
CPPFLAGS = -pedantic -Wall -Werror -Wno-unused-function -Wno-unused-variable
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


$(TARGET): $(OBJECTS)
	$(CPP) $(CPPFLAGS) $(addprefix $(BUILDDIR)/, $(OBJECTS)) -o $(BUILDDIR)/$@ $(LIBS)
	ln -fs build/app app


$(OBJECTS): %.o: %.c 
	$(CPP) -c $(CPPFLAGS) $< -o $(BUILDDIR)/$@ 


clean: FORCE
	rm -rf $(addprefix $(BUILDDIR)/, $(TARGET) $(OBJECTS))


remake: clean all 


FORCE: