#Makefile for Mandelbrot
#Figl, Winterhalder

CPPOPT = /EHsc /D_CRT_SECURE_NO_DEPRECATE
CPPFLAGS = -DESRI_WINDOWS /openmp $(CPPOPT)
CPP = cl.exe

all: mandelbrot

mandelbrot: main.obj tga.obj
	link.exe /out:mandelbrot.exe main.obj tga.obj
	
main.obj: main.cpp
	$(CPP) $(CPPFLAGS) /c main.cpp
	
tga.obj: tga.cpp
	$(CPP) $(CPPFLAGS) /c tga.cpp

clean:
	del mandelbrot.exe main.obj tga.obj
