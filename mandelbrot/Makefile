#Makefile for Mandelbrot
#Figl, Winterhalder

CPPOPT = /EHsc /D_CRT_SECURE_NO_DEPRECATE
CPPFLAGS = -DESRI_WINDOWS /openmp $(CPPOPT)
CPP = cl.exe

all: mandelbrot

mandelbrot: main.obj
	link.exe /out:mandelbrot.exe main.obj
	
main.obj: main.cpp
	$(CPP) $(CPPFLAGS) /c main.cpp

clean:
	del mandelbrot.exe main.obj
