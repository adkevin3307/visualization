EXE = main
OBJ_DIR = obj

SOURCES = $(filter-out test.cpp, $(wildcard *.cpp))

ifeq ($(findstring test, $(MAKECMDGOALS)), test)
	SOURCES = $(filter-out main.cpp, $(wildcard *.cpp))
endif

SOURCES += $(wildcard src/glad/*.c)
SOURCES += $(wildcard src/imgui/*.cpp)
SOURCES += $(wildcard src/implot/*.cpp)
SOURCES += $(wildcard src/*.cpp)

OBJS = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

CXXFLAGS = -std=c++17 -I./include -Wall

LIBS = -L./lib

mkdir =
rm =

ifeq ($(findstring static, $(MAKECMDGOALS)), static)
	CXXFLAGS += -static
endif

ifeq ($(OS), Windows_NT)
	LIBS += -lglfw3 -lopengl32 -lgdi32

	mkdir = if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

	rm = del /S /Q *.exe *.out imgui.ini Data\Scalar\histogram.txt & rmdir /S /Q $(OBJ_DIR)
else ifeq ($(findstring Microsoft, $(shell uname -a)), Microsoft)
	CXX = cmd.exe /C g++
	EXE = main.exe

	LIBS += -lglfw3 -lopengl32 -lgdi32

	mkdir = mkdir -p $(OBJ_DIR)

	rm = rm -rf *.exe *.out imgui.ini $(OBJ_DIR) Data/Scalar/histogram.txt
else
	LIBS += -lglfw3 -pthread -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -ldl -lXcursor

	mkdir = mkdir -p $(OBJ_DIR)

	rm = rm -rf *.exe *.out imgui.ini $(OBJ_DIR) Data/Scalar/histogram.txt
endif

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: src/glad/%.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: src/imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: src/implot/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all static test: create_directory $(EXE) execute
	@echo Compile and Execute Success

create_directory:
	$(call mkdir)

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

execute:
	./$(EXE)

clean:
	$(call rm)
