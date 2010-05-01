all: demo-1 demo-2

demo-2: demo-2.c core.o
	gcc -o demo-2 demo-2.c core.o `pkg-config --cflags --libs sdl cairo`
demo-1: demo-1.c core.o
	gcc -o demo-1 demo-1.c core.o `pkg-config --cflags --libs sdl cairo`
core.o: core.c core.h
	gcc -c core.c `pkg-config --cflags sdl cairo`
