
FLAGS = -std=c++11 -lm -Isrc -w -g -I/usr/local/Cellar/glew/1.12.0/include/GL/ -L `pkg-config --cflags --libs glew`
FLAGS += -F. -framework OpenGL -framework SDL2 -framework SDL2_ttf -framework SDL2_mixer

all:
	g++ $(FLAGS) -o monolith src/monolith.cpp 
