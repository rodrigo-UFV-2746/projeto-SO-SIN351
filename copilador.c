/*
  UNIVERSIDADE FEDERAL DE VIÇOSA - CAMPUS RIO PARANAIBA
  PER 2020-4 PERIODO ESPECIAL REMOTO
  SIN 351 SISTEMAS OPERACIONAIS
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
#define MAX_TAM 512

void imprime_prefixo_shell();
int tratar_linha(char *linha, char **argumentos, char **argumentos_pipe);
int processa_pipe(char *str, char **pipe_dividido);
void processa_espaco(char *str, char **espaco_dividido);
void remove_espaco_desnecessario(char *str);
void verifica_erro(char *comando);
void executa_comando(char **argumentos, int num_comandos, int num_comandos_restantes, int *pipe);
void executa_comando_pipe(char **argumentos_pipe, int qnt_virgulas);

//========================void====== MAIN ================================

int main()
{
	//inicializa variáveis
	int loop = 1, i = 0, num_pipes = 0;
	char linha[MAX_TAM];
	char **argumentos = (char **)malloc(sizeof(char *) * MAX_TAM);
	for (i = 0; i < MAX_TAM; i++) argumentos[i] = (char *)malloc(sizeof(char) * MAX_TAM);
	char **argumentos_pipe = (char **)malloc(sizeof(char *) * MAX_TAM);
	for (i = 0; i < MAX_TAM; i++) argumentos_pipe[i] = (char *)malloc(sizeof(char) * MAX_TAM);

	system("clear");
	//loop principal do programa
	do
	{
		imprime_prefixo_shell();
		fflush(stdin);
		gets(linha);
		num_pipes = tratar_linha(linha, argumentos, argumentos_pipe);
		executa_comando_pipe(argumentos_pipe, num_pipes);
		
	} while (loop);
	return 0;
}

//============================== MAIN ================================

// separa o que foi obtido do usuário por linhas, retorna 0 para comando simples ou >0 para comandos com pipe
int tratar_linha(char *linha, char **argumentos, char **argumentos_pipe)
{
	int i, num_virgulas = 0;

	num_virgulas = processa_pipe(linha, argumentos_pipe);
	
	/* if(num_virgulas == 0 && argumentos_pipe[0] != NULL){
		processa_espaco(argumentos_pipe[0], argumentos);
	} */
	/*
	for(i = 0; argumentos_pipe[i] != NULL; i++){
		printf("str[%d]: %s\n", i, argumentos_pipe[i]);
	}
	printf("\n");
	*/
	return num_virgulas;
}

// remove espaços desnecessários, deixando apenas 1 espaço na string
void remove_espaco_desnecessario(char *str)
{
	//apenas apaga os espaços em branco
	int i, j;
	for (i = 0; i < strlen(str) - 1; i++)
	{
		//printf("str[%d]: %s\n", i, str[i]);
		if (str[i] == ' ' && str[i + 1] == ' ')
		{
				
			for (j = i; j < strlen(str); j++)
			{
				str[j] = str[j + 1];
			}
			i--;
		}
	}
}

// executa o comando efetivamente, direcionando ou não sua entrada/saída para o pipe
void executa_comando(char **argumentos, int num_comandos, int num_comandos_restantes, int *pipe)
{
	//verifica se foi digitado apenas espaços em branco ou vírgulas
	if (argumentos[0] == NULL)
		return;

	//interpreta o comando "quit" como uma tentatia de saída
	if (strcmp(argumentos[0], "quit") == 0 || strcmp(argumentos[0], "QUIT") == 0)
		exit(EXIT_SUCCESS);

	//tenta mudar de diretório ao ler o comando "cd"
	if (strcmp(argumentos[0], "cd") == 0)
	{
		//trata a tentativa de ir para o diretório pai
		if (strcmp(argumentos[1], "..") == 0)
		{
			if (chdir("..") != 0)
				printf("bash: cd: %s: Arquivo ou diretório não encontrado\n", argumentos[1]);
		}
		else
			//trata a tentativa de ir para um diretório qualquer
			if (chdir(argumentos[1]) != 0)
			printf("bash: cd: %s: Arquivo ou diretório não encontrado\n", argumentos[1]);

		return;
	}

	pid_t pid;
	int i = 0;

	//se fork() não conseguir criar um processo
	if ((pid = fork()) < 0)
	{
		perror("Erro em fork");
		exit(EXIT_FAILURE);
	}

	//este bloco será executado pelo processo FILHO
	if (pid == 0)
	{
		//comando simples SEM pipe
		if(num_comandos == 1){
			if(execvp(argumentos[0], argumentos)){
				perror("Erro no execvp0");
			}
		}
		
		//comandos COM pipe
		//somente escreve no pipe
		if(num_comandos_restantes+1 == num_comandos){
			if(close(pipe[0]) < 0){
				perror("Erro em close pipe1[0]");
			}
			
			if(dup2(pipe[1], STDOUT_FILENO) < 0){
				perror("Error em dup2_2");
			}
			/* if(close(pipe[1]) < 0){
				perror("Erro em close pipe1[1]");
			} */
			if(execvp(argumentos[0], argumentos)){
				perror("Erro no execvp1");
			}
		}else
		//somente lê do pipe
		if(num_comandos_restantes == 0){
			if(close(pipe[1]) < 0){
				perror("Erro em close pipe2[1]");
			}
			if(dup2(pipe[0], STDIN_FILENO) < 0){
				perror("Error em dup2_2");
			}
			/* if(close(pipe[0]) < 0){
				perror("Erro em close pipe2[0]");
			} */

			if(execvp(argumentos[0], argumentos)){
				perror("Erro no execvp2");
			}

		}else
		if(num_comandos_restantes > 0){
			//comando que está entre dois pipes
			if(dup2(pipe[0], STDIN_FILENO) < 0){
				perror("Error em dup2_3");
			}
			if(dup2(pipe[1], STDOUT_FILENO) < 0){
				perror("Error em dup2_3");
			}
		}

		//verifica erros
		verifica_erro(argumentos[0]);
	}
}

// BACKUP da função que escrevemos antes de implementar o pipe
/*{
void executa_comando(char **argumentos)
{
	//verifica se foi digitado apenas espaços em branco
	if (argumentos[0] == NULL)
	{
		return;
	}
	//interpreta o comando "quit" como uma tentatia de saída
	if (strcmp(argumentos[0], "quit") == 0 || strcmp(argumentos[0], "QUIT") == 0)
		exit(EXIT_SUCCESS);

	//tenta mudar de diretório ao ler o comando "cd"
	if (strcmp(argumentos[0], "cd") == 0)
	{
		//trata a tentativa de ir para o diretório pai
		if (strcmp(argumentos[1], "..") == 0)
		{
			if (chdir("..") != 0)
				printf("bash: cd: %s: Arquivo ou diretório não encontrado\n", argumentos[1]);
		}
		else
			//trata a tentativa de ir para um diretório qualquer
			if (chdir(argumentos[1]) != 0)
			printf("bash: cd: %s: Arquivo ou diretório não encontrado\n", argumentos[1]);

		return;
	}

	pid_t pid;
	int i = 0;

	//se fork() não conseguir criar um processo
	if ((pid = fork()) < 0)
	{
		perror("Erro em fork");
		exit(EXIT_FAILURE);
	}

	//este bloco será executado pelo processo FILHO
	if (pid == 0)
	{
		execvp(argumentos[0], argumentos);
		
		//verifica erros
		verifica_erro(argumentos[0]);
	}

	//este bloco será executado pelo processo PAI
	if (pid > 0)
	{
		int retorno, status;
		do
		{
			//espera o fim da execução do processo filho
			retorno = waitpid(pid, &status, WUNTRACED | WCONTINUED);

			if (retorno < 0)
			{
				perror("Erro em waitpid");
				exit(EXIT_FAILURE);
			}

		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		//exit(EXIT_SUCCESS);
	}
}
}*/

// com os comandos lidos, esta função inicializa o pipe e fornece os parâmetros para a funçao executa_comando
void executa_comando_pipe(char **argumentos_pipe, int qnt_virgulas)
{
	if(argumentos_pipe[0] == NULL)
		return;
	int i;
	int num_comandos = qnt_virgulas+1; // num_comandos = quantidade total de comandos para executar
	int num_comandos_restantes = num_comandos; // num_comandos_restantes = quantidade de comandos que ainda não foram executados
	int fd[2];
	char **argumentos = (char **)malloc(sizeof(char *) * MAX_TAM);
	for (i = 0; i < MAX_TAM; i++)
		argumentos[i] = (char *)malloc(sizeof(char) * MAX_TAM);

	pipe(fd);
	// não usa pipe
	if(num_comandos == 1){
		processa_espaco(argumentos_pipe[0], argumentos);
		executa_comando(argumentos, num_comandos, --num_comandos_restantes, fd);
	}else
	// usa o pipe
	for(i = 0; i < num_comandos; i++){
		processa_espaco(argumentos_pipe[i], argumentos);
		executa_comando(argumentos, num_comandos, --num_comandos_restantes, fd);
	}
	// fecha a entrada e saída do pipe
	close(fd[0]);
	close(fd[1]);

	// o processo pai aguarda o fim da execução dos processos filhos
	for(i = 0; i < num_comandos; i++){
		wait(NULL);
	}
}

// função verifica erros colocado todos os erros da lista de erro da função execvp();
void verifica_erro(char *comando)
{
	if (errno == ENOENT)
	{
		printf("%s: comando ou diretório inválido.\n", comando);
	}
	if (errno == E2BIG)
	{
		printf("%s: E2BIG The total number of bytes in the environment (envp) and argument list (argv) is toolarge.\n", comando);
	}
	if (errno == EACCES)
	{
		printf("%s:EACCES Search  permission  is  denied on a component of the path prefix of pathname or the name of a script interpreter.  (See also path_resolution(7).)\n", comando);
	}
	if (errno == EACCES)
	{
		printf("%s: EACCES The file or a script interpreter is not a regular file.\n", comando);
	}
	if (errno == EACCES)
	{
		printf("%s: EACCES Execute permission is denied for the file or a script or ELF interpreter.\n", comando);
	}
	if (errno == EACCES)
	{
		printf("%s: EACCES The filesystem is mounted noexec.\n", comando);
	}
	if (errno == EAGAIN)
	{
		printf("%s: EAGAIN (since Linux 3.1) Having changed its real UID using one of the set*uid() calls, the caller was—and is now  still—above  its  RLIMIT_NPROC  resource limit (see setrlimit(2)).  For a more detailed explanation of this error, see NOTES..\n", comando);
	}
	if (errno == EFAULT)
	{
		printf("%s: EFAULT pathname or one of the pointers in the vectors argv or  envp  points  outside  your accessible address space.\n", comando);
	}
	if (errno == EINVAL)
	{
		printf("%s: EINVAL An  ELF  executable  had  more than one PT_INTERP segment (i.e., tried to name more than one interpreter).\n", comando);
	}
	if (errno == EIO)
	{
		printf("%s:EIO    An I/O error occurred.\n", comando);
	}
	if (errno == EISDIR)
	{
		printf("%s:EISDIR An ELF interpreter was a directory.\n", comando);
	}
	if (errno == ELIBBAD)
	{
		printf("%s:ELIBBAD An ELF interpreter was not in a recognized format.\n", comando);
	}
	if (errno == ELOOP)
	{
		printf("%s:ELOOP  Too many symbolic links were encountered in resolving pathname or  the  name  of  a script or ELF interpreter.\n", comando);
	}
	if (errno == ELOOP)
	{
		printf("%s:The maximum recursion limit was reached during recursive script interpretation (see Interpreter scripts , above).  Before Linux 3.8, the error produced for this  case was ENOEXEC.\n", comando);
	}
	if (errno == EMFILE)
	{
		printf("%s:EMFILE The per-process limit on the number of open file descriptors has been reached.\n", comando);
	}
	if (errno == ENAMETOOLONG)
	{
		printf("%s:ENAMETOOLONG pathname is too long.\n", comando);
	}
	if (errno == ENFILE)
	{
		printf("%s:ENFILE The system-wide limit on the total number of open files has been reached.\n", comando);
	}
	if (errno == ENOEXEC)
	{
		printf("%s:ENOEXEC An  executable is not in a recognized format, is for the wrong architecture, or has some other format error that means it cannot be executed.\n", comando);
	}
	if (errno == ELIBBAD)
	{
		printf("%s:ELIBBAD An ELF interpreter was not in a recognized format.\n", comando);
	}
	if (errno == ENOMEM)
	{
		printf("%s:ENOMEM Insufficient kernel memory was available.\n", comando);
	}
	if (errno == ENOTDIR)
	{
		printf("%s:ENOTDIR A component of the path prefix of pathname or a script or ELF interpreter is not a directory.\n", comando);
	}
	if (errno == EPERM)
	{
		printf("%s:EPERM The filesystem is mounted nosuid, the user is not the superuser, and the file has the set - user - ID or set - group - ID bit set.\n", comando);
	}
	if (errno == EPERM)
	{
		printf("%s:EPERM The process is being traced, the user is not the superuser and the file has the set - user - ID or set - group - ID bit set.\n", comando);
	}
	if (errno == EPERM)
	{
		printf("%s: EPERM  A  capability -dumb   applications  would  not  obtain  the  full  set of permitted capabilities granted by the executable file.  See capabilities(7).\n", comando);
	}
	exit(EXIT_FAILURE);
}

// serve apenas para imprimir o prefixo e o diretório atual do shell
void imprime_prefixo_shell()
{
	char dir[1024];
	getcwd(dir, sizeof(dir));
	printf("meu@shell:%s$ ", dir);
}

//separa os comandos divididos por vírgulas, retornando o número de vírgulas encontradas
int processa_pipe(char *str, char **pipe_dividido)
{
	int i = 0;

	char *rest = NULL;
	char *token;

	token = strtok_r(str, ",", &rest);
	while (token != NULL)
	{
		pipe_dividido[i] = token;
		//printf("token: %s , %d\n", token, getpid());
		token = strtok_r(NULL, ",", &rest);
		i++;
	}

	pipe_dividido[i] = NULL;
	return i > 0 ? i - 1 : 0;
}

//separa o comando de seus parâmetros para armazenar em uma lista de strings
void processa_espaco(char *str, char **espaco_dividido)
{
	remove_espaco_desnecessario(str);
	int i = 0;

	char *rest = NULL;
	char *token;
	token = strtok_r(str, " ", &rest);
	while (token != NULL)
	{
		espaco_dividido[i] = token;
		//printf("token: '%s'\n", token);
		token = strtok_r(NULL, " ", &rest);
		i++;
	}
	espaco_dividido[i] = NULL;
}
