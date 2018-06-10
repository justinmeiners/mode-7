SDLINCLUDE=/usr/local/include
SDLLIB=/usr/local/lib

FLAGS=-O2
LIBS=-lSDL2 -framework OpenGL
IFLAGS=-I${SDLINCLUDE} -L${SDLLIB}

SOURCES=*.c

mode-7: ${SOURCES}
	gcc ${FLAGS} ${IFLAGS} ${LIBS} $^ -o $@
