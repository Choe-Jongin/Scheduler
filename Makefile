scheduler : main.o
	gcc -o scheduler main.o -g

main.o : main.c
	gcc -c -o main.o main.c

clean:
	rm *.o scheduler	
