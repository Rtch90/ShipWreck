CC = g++
CFLAGS = -ansi -Wall -g
LDADD = -lGL -lGLU -lpthread -lSDL -lSDL_image -lopenal -lalut -lvorbisfile -L/usr/X11
objects = BoundBox.o Camera.o City.o CityEconomy.o CityManager.o Config.o \
          Economy.o EnvironmentObject.o Frustum.o GameState.o Log.o MainMenu.o \
          Map.o Menu.o Model.o Ocean.o Player.o Point.o Product.o QuadBox.o \
          QuadNode.o Ships.o SkyBox.o StateBattle.o StateCity.o \
          StateSailing.o Terrain.o Wave.o WaveEmitter.o collisions.o \
          extensions.o font.o image.o normals.o Cargo.o \
          tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o
          

.PHONY: default all clean

default: all

%.cpp: %.h

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(objects)

clean:
	rm -f $(objects)
