/*
  UNIVERSIDADE FEDERAL DE VISOCA - CAMPUS RIO PARANAIBA
  PER 2020-4 PERIODO ESPECIAL REMOTO
  SIN 351 SISTEMAS OPERACIONAL
  PROFESSOR: RODRIGO MOREIRA
  Trabalho Prático I: Construindo um Interpretador Shell
  ALUNOS: 2746 RODRIGO DE OLIVEIRA COSTA
          6010 JOAO VITOR AZEVEDO     
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
int cria_processo(){
	return 0;
}
int executacomando(){
	return 0;
}
int main(){
    int loop = 1;
	char comando[512] = "";
	do{	
		printf("meu-shell> ");
		fflush(stdin);
		gets(comando);
    	if(strcmp (comando, "exit") == 0 ){
    		print("teste");
    		return 0;
		}
	}while(loop != 0);
    return 0;
}
