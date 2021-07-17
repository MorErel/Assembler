assembler: firstPass.o secondPass.o fileUtil.o baseUtil.o data.o compiler.o main.o parsing.o utilities.o header.h
	gcc -g -Wall -ansi -pedantic firstPass.o secondPass.o fileUtil.o baseUtil.o data.o compiler.o main.o parsing.o utilities.o -o assembler
main.o: main.c
	gcc -c -Wall -ansi -pedantic main.c -o main.o
firstPass.o: firstPass.c header.h
	gcc -c -Wall -ansi -pedantic firstPass.c -o firstPass.o
secondPass.o: secondPass.c header.h
	gcc -c -Wall -ansi -pedantic secondPass.c -o secondPass.o
fileUtil.o: fileUtil.c header.h
	gcc -c -Wall -ansi -pedantic fileUtil.c -o fileUtil.o
baseUtil.o: baseUtil.c header.h
	gcc -c -Wall -ansi -pedantic baseUtil.c -o baseUtil.o
data.o: data.c header.h
	gcc -c -Wall -ansi -pedantic data.c -o data.o
compiler.o: compiler.c header.h
	gcc -c -Wall -ansi -pedantic compiler.c -o compiler.o
parsing.o: parsing.c header.h
	gcc -c -Wall -ansi -pedantic parsing.c -o parsing.o
utilities.o: utilities.c header.h
	gcc -c -Wall -ansi -pedantic utilities.c -o utilities.o

