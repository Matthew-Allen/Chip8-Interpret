SRCDIR = src/
INCLDIR = include/
GL3WDIR = include/GL/
BUILDDIR = build/
ODIR = $(BUILDDIR)objs/
CIMGUIDIR = extern/cimgui/
IMGUIDIR = extern/cimgui/imgui/
CIMGUI_LIB = cimgui.so

CSRC := $(wildcard $(SRCDIR)*.c) \
	$(wildcard $(INCLDIR)*.c) \
	$(wildcard $(GL3WDIR)*.c)
CPPSRC := $(wildcard $(INCLDIR)*.cpp)
OBJS := $(notdir $(CPPSRC:.cpp=.o))
OBJS += $(notdir $(CSRC:.c=.o))
OBJS := $(addprefix $(ODIR),$(OBJS))
BUILDC = gcc -c -lSDL2 -lGL -ldl `sdl2-config --cflags`
BUILDCPP = g++ -c -lSDL2 -lGL -ldl `sdl2-config --cflags`
LINK = g++ -lSDL2 -lGL -ldl $(CIMGUI_LIB)
CFLAGS = -g -DIMGUI_IMPL_API="extern" -DIMGUI_IMPL_OPENGL_LOADER_GL3W -I$(SRCDIR) -I$(GL3WDIR) -I$(INCLDIR) -I$(CIMGUIDIR) -I$(IMGUIDIR)
CPPFLAGS = -g -DIMGUI_IMPL_API="extern \"C\"" -DIMGUI_IMPL_OPENGL_LOADER_GL3W -I$(SRCDIR) -I$(GL3WDIR) -I$(INCLDIR) -I$(CIMGUIDIR) -I$(IMGUIDIR)

.PHONY: build
.PHONY: clean

build: cimgui.so $(BUILDDIR)Chip8-Interpret.out

$(BUILDDIR)Chip8-Interpret.out: $(OBJS)
	$(LINK) -o $@ $^

$(ODIR)%.o:: $(SRCDIR)%.c
	$(BUILDC) $(CFLAGS) $^ -o $@

$(ODIR)%.o:: $(INCLDIR)%.c
	$(BUILDPP) $(CFLAGS) $^ -o $@

$(ODIR)%.o:: $(SRCDIR)%.cpp
	$(BUILDC) $(CPPFLAGS) $^ -o $@

$(ODIR)%.o:: $(INCLDIR)%.cpp
	$(BUILDCPP) $(CPPFLAGS) $^ -o $@

$(ODIR)%.o:: $(GL3WDIR)%.c
	$(BUILDC) $(CFLAGS) $^ -o $@

cimgui.so:
	make -C extern/cimgui
	cp extern/cimgui/$(CIMGUI_LIB) ./

clean:
	rm -f $(ODIR)*.o
	rm -f $(BUILDDIR)*.out

