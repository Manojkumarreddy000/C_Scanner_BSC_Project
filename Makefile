# Make file for C Project
test.out : functions_shaarad.o functions_manoj.o test.o
	gcc -g -o test.out functions_shaarad.o functions_manoj.o test.o

# Functions written by Shaarad
functions_shaarad.o : functions_shaarad.c header.h
	gcc -g -c functions_shaarad.c

# Functions written by Manoj
functions_manoj.o : functions_manoj.c header.h
	gcc -g -c functions_manoj.c

# Main test function
test.o : test.c header.h
	gcc -g -c test.c

cl : 
	rm -f *.o
	rm -f *.txt
	rm -f *.out

clear : 
	rm -f *.o
	rm -f *.txt
	rm -f *.out


