args = -lglfw3 -lopengl32 -lgdi32 -Wall
remove_command =

ifeq ($(OS), Windows_NT)
	remove_command += del *.exe &
	remove_command += del Data\Scalar\histogram.txt
else
	remove_command += rm -f *.exe &
	remove_command += rm -f ./Data/Scalar/histogram.txt
endif

default: compile execute

compile:
	cmd.exe /C g++ -I./include/ $(filter-out test.cpp, $(wildcard *.cpp)) src/glad.c -L./lib/ $(args) -o main.exe

histogram:
	cmd.exe /C g++ -DHISTOGRAM -I./include/ $(filter-out test.cpp, $(wildcard *.cpp)) src/glad.c -L./lib/ $(args) -o main.exe

execute:
	cmd.exe /C main.exe

clean:
	$(remove_command)
