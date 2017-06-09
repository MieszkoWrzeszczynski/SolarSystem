CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix src/,$(notdir $(CPP_FILES:.cpp=.o)))
LD_FLAGS := -lm -lGL -lGLU -lglut -lGLEW
CC_FLAGS := -Wall -g -std=c++0x

main: $(OBJ_FILES)
	g++ $(CC_FLAGS) -o $@ $^  $(LD_FLAGS)

obj/%.o: src/%.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

clean:
	rm -rf src/*.obj