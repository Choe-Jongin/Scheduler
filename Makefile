scheduler : main.o
	gcc -o scheduler main.o

main.o : main.c
	gcc -c -o main.o main.c

clean:
	rm *.o scheduler	
