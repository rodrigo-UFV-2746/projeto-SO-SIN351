# Simulador de Shell
all: copilador.c 
	gcc -g -Wall -o shell copilador.c
clean:
	$(RM) shell
