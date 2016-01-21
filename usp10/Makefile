default: all

test_ScriptStringOut_with_proportional_font.o: test_ScriptStringOut_with_proportional_font.c
	i686-w64-mingw32-gcc -O0 -g -gdwarf-2 -gstrict-dwarf -loleaut32 -c -o test_ScriptStringOut_with_proportional_font.o test_ScriptStringOut_with_proportional_font.c -Wall

test_ScriptStringOut_with_proportional_font.exe: test_ScriptStringOut_with_proportional_font.o
	i686-w64-mingw32-gcc -O0 -g -gdwarf-2 -gstrict-dwarf -loleaut32 -o test_ScriptStringOut_with_proportional_font.exe -s test_ScriptStringOut_with_proportional_font.o -mwindows -lusp10

all: test_ScriptStringOut_with_proportional_font.exe

run:
	wine test_ScriptStringOut_with_proportional_font.exe
clean:
	find . -name "*.o" -exec rm -f {} \;
	find . -name "*~" -exec rm -f {} \;
	find . -name "*.exe" -exec rm -f {} \;
