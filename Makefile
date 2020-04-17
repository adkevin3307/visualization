LIBS = -L./lib -lglfw3 -lopengl32 -lgdi32
LINUX_LIBS = -L./lib -lglfw3 -pthread -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -ldl -lXcursor

CXXFLAGS = -I./include -I./include/imgui -Wall

SOURCE = $(filter-out test.cpp, $(wildcard *.cpp)) src/glad/* src/imgui/* src/*.cpp
SOURCE_TEST = $(filter-out main.cpp, $(wildcard *.cpp)) src/glad/* src/imgui/* src/*.cpp

remove_command =

ifeq ($(OS), Windows_NT)
	remove_command += del *.exe &
	remove_command += del imgui.ini &
	remove_command += del Data\Scalar\histogram.txt
else
	remove_command += rm -f *.exe *.out &
	remove_command += rm -f imgui.ini &
	remove_command += rm -f ./Data/Scalar/histogram.txt
endif

default: compile execute

compile:
	cmd.exe /C g++ $(CXXFLAGS) $(SOURCE) $(LIBS) -o main.exe

histogram:
	cmd.exe /C g++ -DHISTOGRAM $(CXXFLAGS) $(SOURCE) $(LIBS) -o main.exe

execute:
	cmd.exe /C main.exe

test:
	cmd.exe /C g++ $(CXXFLAGS) $(SOURCE_TEST) $(LIBS) -o test.exe
	cmd.exe /C test.exe

linux:
	g++ $(CXXFLAGS) $(SOURCE) $(LINUX_LIBS) -o main.out
	./main.out

clean:
	$(remove_command)
