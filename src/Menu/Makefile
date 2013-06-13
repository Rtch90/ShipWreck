CC = g++
CFLAGS = -ansi -Wall -g
LDADD = -lGL -lGLU -lpthread -lSDL -lSDL_image -lopenal -lalut -lvorbisfile -L/usr/X11
objects = Button.o DataContainer.o MenuContainer.o

.PHONY: default all clean

default: all

%.cpp: %.h

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(objects)

clean:
	rm -f $(objects)
