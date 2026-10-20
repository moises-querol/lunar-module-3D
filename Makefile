# Cross-platform build (Linux + macOS)
UNAME_S := $(shell uname -s)

CC      = gcc
CFLAGS  = -Wall -Wextra -O2
LDLIBS  = -lm

SRC_DIR := src
OBJ_DIR := build
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXEC := lunar-module
.DEFAULT_GOAL := all

-include $(OBJ:.o=.d)

ifeq ($(UNAME_S),Darwin)
  # macOS: Homebrew freeglut when available, otherwise Apple GLUT frameworks.
  ifneq ($(shell pkg-config --exists glut 2>/dev/null && echo 1),)
    CFLAGS += -DUSE_FREEGLUT $(shell pkg-config --cflags glut)
    LDLIBS += $(shell pkg-config --libs glut) -framework OpenGL
  else
    LDLIBS += -framework OpenGL -framework GLUT
  endif
else
  # Linux: pkg-config when available, otherwise typical Mesa/freeglut paths.
  ifneq ($(shell pkg-config --exists gl glu glut 2>/dev/null && echo 1),)
    CFLAGS += $(shell pkg-config --cflags gl glu glut)
    LDLIBS += $(shell pkg-config --libs gl glu glut)
  else
    CFLAGS += -I/usr/include/GL
    LDLIBS += -lGL -lGLU -lglut
  endif
endif

.PHONY: all clean check-deps

all: check-deps $(EXEC)

check-deps:
ifeq ($(UNAME_S),Darwin)
	@if pkg-config --exists glut 2>/dev/null; then \
		:; \
	elif [ -d "/System/Library/Frameworks/GLUT.framework" ] \
	  && [ -d "/System/Library/Frameworks/OpenGL.framework" ]; then \
		:; \
	else \
		echo "Error: OpenGL/GLUT not found."; \
		echo ""; \
		echo "Install freeglut with Homebrew:"; \
		echo "  brew install freeglut"; \
		echo ""; \
		echo "Then run make again."; \
		exit 1; \
	fi
else
	@if pkg-config --exists gl glu glut 2>/dev/null; then \
		:; \
	elif [ -f /usr/include/GL/glut.h ] \
	  && printf 'int main(void) { return 0; }' | $(CC) -o /dev/null -x c - -lGL -lGLU -lglut -lm 2>/dev/null; then \
		:; \
	else \
		echo "Error: OpenGL/GLUT development libraries not found."; \
		echo ""; \
		echo "Debian/Ubuntu:"; \
		echo "  sudo apt install freeglut3-dev libgl1-mesa-dev libglu1-mesa-dev"; \
		echo ""; \
		echo "Fedora/RHEL:"; \
		echo "  sudo dnf install freeglut-devel mesa-libGL-devel mesa-libGLU-devel"; \
		echo ""; \
		echo "Then run make again."; \
		exit 1; \
	fi
endif

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(EXEC) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(EXEC)
