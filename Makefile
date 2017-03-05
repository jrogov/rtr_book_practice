CXX = g++
CXXFLAGS = -pedantic -Wall -Werror 
CXXFLAGS_DEBUG_BUILD := -Wno-unused-function -Wno-unused-variable
CXXFLAGS_DEBUG_LINK :=
LIBS = -lSDL2 -lGLEW -lGL -lGLU -lm

PVSMESSAGE_1 := // This is a personal academic project. Dear PVS-Studio, please check it.
PVSMESSAGE_2 := // PVS-Studio Static Code Analyzer for C, C++ and C\#: http://www.viva64.com

vpath %.c src $(wildcard src/*/.)
vpath %.o build

SOURCEDIR = src
BUILDDIR = build

IGNORE := camera.c movement.c 
SOURCES := $(filter-out $(IGNORE), $(notdir $(wildcard $(SOURCEDIR)/*.c $(SOURCEDIR)/*/*.c)) )
OBJECTS := $(SOURCES:.c=.o)

TARGET = app


all: $(TARGET)


release : FORCE disable_debug all

disable_debug :
	CXXFLAGS_DEBUG_BUILD := 
	CXXFLAGS_DEBUG_LINK :=



$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_DEBUG_LINK) $(addprefix $(BUILDDIR)/, $(OBJECTS)) -o $(BUILDDIR)/$@ $(LIBS)
	ln -fs build/app app


$(OBJECTS): %.o: %.c 
	$(CXX) -c $(CXXFLAGS) $(CXXFLAGS_DEBUG_BUILD) $< -o $(BUILDDIR)/$@ 



clean: FORCE
	rm -rf $(addprefix $(BUILDDIR)/, $(TARGET) $(OBJECTS))

remake: clean all 

checkpvs:
	

addpvs: $(SOURCES)
	for i in $(SOURCES); do \
	find $(SOURCEDIR) -name $$i -exec \
	sed -i -e "1i $(PVSMESSAGE_1)\n$(PVSMESSAGE_2)\n" {} +; \
	done;

rmpvs: $(SOURCES)
	for i in $(SOURCES); do \
	find $(SOURCEDIR) -name $$i -exec \
	sed -i -e "/PVS-Studio/,+2d;" {} +; \
	done;



FORCE: