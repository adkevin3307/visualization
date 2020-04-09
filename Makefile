args = -lglfw3 -lopengl32 -lgdi32 -Wall
linux_args = -lglfw3 -pthread -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -ldl -lXcursor

remove_command =

ifeq ($(OS), Windows_NT)
	remove_command += del *.exe &
	remove_command += del Data\Scalar\histogram.txt
else
	remove_command += rm -f *.exe *.out &
	remove_command += rm -f ./Data/Scalar/histogram.txt
endif

default: compile execute

compile:
	cmd.exe /C g++ -I./include/ $(filter-out test.cpp, $(wildcard *.cpp)) src/glad.c -L./lib/ $(args) -o main.exe

histogram:
	cmd.exe /C g++ -DHISTOGRAM -I./include/ $(filter-out test.cpp, $(wildcard *.cpp)) src/glad.c -L./lib/ $(args) -o main.exe

execute:
	cmd.exe /C main.exe

linux:
	g++ -I./include/ $(filter-out test.cpp, $(wildcard *.cpp)) src/glad.c -L./lib/ $(linux_args) -o main.out
	./main.ou

clean:
	$(remove_command)
