all: myshell

myshell: myshell.o LineParser.o
	gcc -m32 -g -Wall -o myshell myshell.o LineParser.o

myshell.o: myshell.c
	gcc -m32 -g -Wall -c myshell.c

LineParser.o: LineParser.c
	gcc -m32 -g -Wall -c LineParser.c

clean:
	rm -f *.o myshell
