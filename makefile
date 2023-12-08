# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99

# Source files
SRC = main.c

# Executable name
EXE = game

# SDL2 libraries
SDL_LIBS = $(shell sdl2-config --libs) -lSDL2_ttf -lSDL2_mixer

# Build target
all: $(EXE)

# Linking
$(EXE): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(SDL_LIBS)

# Cleaning
clean:
	rm -f $(EXE)
