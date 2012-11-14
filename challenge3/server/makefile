all:
	gcc -g -ggdb -fno-stack-protector -c server.c
	gcc -fno-stack-protector -c auth.c
	gcc -fno-stack-protector -o server server.o auth.o
clean:
	rm -f *.o 
