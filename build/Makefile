CC = g++
CFLAGS = -ansi -Wall -g
LDADD = -lGL -lGLU -lpthread -lSDL -lSDL_image -lopenal -lalut -lvorbisfile -L/usr/X11
LDADDSTATIC = -Wl,-Bstatic -lSDL -lSDL_image -lopenal -lalut -L/usr/X11 -Wl,-Bdynamic -lasound -lartsc -lesd -lpulse -lpulse-simple -ldirectfb -lvga -laa -lcaca -ljpeg -ltiff -lpthread -lGL -lGLU -Wl,-Bdynamic

.PHONY: default static all clean

default: all

all:
	$(MAKE) -C ../src/Menu
	$(MAKE) -C ../src/StateBattle
	$(MAKE) -C ../src
	$(CC) $(CFLAGS) -o Lephisto ../src/main.cpp ../src/Menu/*.o ../src/StateBattle/*.o ../src/*.o $(LDADD)

static:
	@echo -e "\033[1;31mThis is an experimental build, if it does not work, don't complain...\033[0m"
	@sleep 1
	$(MAKE) -C ../src/Menu
	$(MAKE) -C ../src
	$(CC) $(CFLAGS) -o build/Lephisto-static ../src/main.cpp ../src/Menu/*.o ../src/StateBattle/*.o ../src/*.o $(LDADDSTATIC)

clean:
	$(MAKE) -C ../src/Menu clean
	$(MAKE) -C ../src/StateBattle clean
	$(MAKE) -C ../src clean
	rm -f Lephisto Lephisto.log
