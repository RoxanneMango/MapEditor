.PHONY : all run clean play

# change to your own SFML library path if you want to compile
SFML = C:\Users\Roxanne\Documents\GAMES_\SFML-2.5.1

BUILD = ..\bin

INCLUDE = -I$(SFML)\include
EXEC_LINUX = mapEditor
EXEC = $(BUILD)\$(EXEC_LINUX).exe

LIB = $(SFML)\build\lib
LIBS = $(LIB)\libsfml-graphics.a $(LIB)\libsfml-system.a $(LIB)\libsfml-window.a $(LIB)\libsfml-main.a

all:
	@echo "Making object files. . ."
	@g++ -c *.cpp -Wl,--whole-archive $(INCLUDE) -std=c++11 
	@echo "Doing stuff with object files. . ."
#	@g++ *.o -o $(EXEC) $(LIBS) -static -mwindows
#	@g++ *.o -o $(EXEC) $(LIBS) -static franken_patch.res -mwindows
	@g++ *.o -o $(EXEC) $(LIBS) -static franken_patch.res
	@$(EXEC)

clean: 
	@echo "Removing object files again. . ."
	@del *.o

play:
	@$(EXEC)