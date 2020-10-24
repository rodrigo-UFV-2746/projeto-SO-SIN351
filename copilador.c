/*
  UNIVERSIDADE FEDERAL DE VIÇOSA - CAMPUS RIO PARANAIBA
  PER 2020-4 PERIODO ESPECIAL REMOTO
  SIN 351 SISTEMAS OPERACIONAL
  PROFESSOR: RODRIGO MOREIRA
  Trabalho Prático I: Construindo um Interpretador Shell
  ALUNOS: 2746 RODRIGO DE OLIVEIRA COSTA
          6010 JOAO VITOR AZEVEDO     
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

void tratar_linha(char *linha, char **argumentos);
void remove_espaco_desnecessario(char *str);
int executa_comando();

//============================== MAIN ================================

int main(){
	int loop = 1;
	char linha[513] = "";
	char *argumentos[64];
	
	do{	
		printf("meu-shell> ");
		fflush(stdin);
		gets(linha);
		if(strcmp (linha, "exit") == 0 ){
			printf("teste");
			return 0;
		}
		tratar_linha(linha, argumentos);
		executa_comando();
		
	}while(loop != 0);
    return 0;
}

//============================== MAIN ================================

void tratar_linha(char *linha, char **argumentos){
	//esta função irá separar a string char *linha em substrings que são armazenadas em char **argumentos
	int i = 0;
	
	remove_espaco_desnecessario(linha);
	
	//esta string auxiliar é criada pq a função strtok alteraria a string original
	char aux_linha[strlen(linha)+1];
	strcpy(aux_linha, linha);
	
	char *ptr = strtok(aux_linha, " ");
	//cada posição de argumentos[i] receberá uma substring de char *linha
	while(ptr != NULL){
		argumentos[i] = ptr;
		printf("'%s'\n", ptr);
		ptr = strtok(NULL, " ");
		i++;
	}
	//o último elemento de char **argumentos é NULL para ficar de acordo com o exigido pela função execvp
	argumentos[i] = NULL;
	
	
}

void remove_espaco_desnecessario(char *str){
	//apenas apaga os espaços em branco
	int i, j;
	for(i = 0; i < strlen(str) - 1; i++){
		if(str[i] == ' ' && str[i+1] == ' '){
			for(j = i; j < strlen(str); j++){
				str[j] = str[j+1];
			}
			i--;
		}
	}
}

int executa_comando(){
	return 0;
}
