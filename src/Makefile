
LIBS =  -lm -lGL -lGLU -lglut -lGLEW

all: main

main: main.o Camera.o Shader_Loader.o Render_Utils.o Texture.o picopng.o 
	g++ -Wall -g -std=c++0x  -o main main.o Camera.o Shader_Loader.o Render_Utils.o Texture.o picopng.o  $(LIBS) 

SRC=$(wildcard *.cpp)

test: $(SRC)
	g++ -Wall -g -c $? -o $@  -std=c++0x

clean:
	rm -f *.o main


