default: all

smaple_program.o: smaple_program.c
	i686-w64-mingw32-gcc -O0 -g -gdwarf-2 -gstrict-dwarf -loleaut32 -c -o smaple_program.o smaple_program.c -Wall 

smaple_program.exe: smaple_program.o
	i686-w64-mingw32-gcc -O0 -g -gdwarf-2 -gstrict-dwarf -loleaut32 -o smaple_program.exe -s smaple_program.o -mwindows

all: smaple_program.exe

clean:
	find . -name "*.o" -exec rm -f {} \;
	find . -name "*~" -exec rm -f {} \;
	find . -name "*.exe" -exec rm -f {} \;
