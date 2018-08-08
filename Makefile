CXX = g++

all: win_test.exe

win_test.exe: main.o
	g++ main.o -o win_test.exe -DSFML_STATIC -L /c/mingw64/usr/SFML-2.5.0/lib/ -lsfml-graphics-s -lsfml-window-s -lsfml-audio-s -lsfml-system-s -lwinmm -lgdi32 -lopengl32 -lfreetype -lopenal32 -lflac -lvorbisenc -lvorbisfile -lvorbis -logg 

main.o: main.cpp
	g++ -c main.cpp -I /c/mingw64/usr/SFML-2.5.0/include/ 
