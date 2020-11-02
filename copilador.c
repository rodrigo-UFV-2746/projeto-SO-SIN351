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

void tratar_linha(char *linha, char **argumentos);
void remove_espaco_desnecessario(char *str);
void verifica_erro(char *comando);
void executa_comando();

//============================== MAIN ================================

int main()
{
	int loop = 1;
	char linha[512];
	char *argumentos[512];
	char *dir = (char*) calloc(1024, sizeof(char));
	system("clear");
	do
	{
		getcwd(dir, 1024);
		printf("meu@shell:%s$ ", dir);
		gets(linha);
		fflush(stdin);
		tratar_linha(linha, argumentos);
		executa_comando(argumentos);
	
	} while (loop != 0);
	return 0;
}

//============================== MAIN ================================

void tratar_linha(char *linha, char **argumentos)
{
	//esta função irá separar a string char *linha em substrings que são armazenadas em char **argumentos
	int i = 0;

	remove_espaco_desnecessario(linha);

	//esta string auxiliar é criada pq a função strtok alteraria a string original
	char aux_linha[strlen(linha) + 1];
	strcpy(aux_linha, linha);

	char *ptr = strtok(aux_linha, " ");
	//cada posição de argumentos[i] receberá uma substring de char *linha
	while (ptr != NULL)
	{
		argumentos[i] = ptr;
		//printf("'%s'\n", ptr);
		ptr = strtok(NULL, " ");
		i++;
	}
	//o último elemento de char **argumentos é NULL para ficar de acordo com o exigido pela função execvp
	argumentos[i] = NULL;
}

void remove_espaco_desnecessario(char *str)
{
	//apenas apaga os espaços em branco
	int i, j;
	for (i = 0; i < strlen(str) - 1; i++)
	{
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

void executa_comando(char **argumentos)
{
	//interpreta o comando "quit" como uma tentatia de saída
	if (strcmp(argumentos[0], "quit") == 0)
		exit(EXIT_SUCCESS);

	//tenta mudar de diretório ao ler o comando "cd"
	if (strcmp(argumentos[0], "cd") == 0){
		//trata a tentativa de ir para o diretório pai
		if(strcmp(argumentos[1], "..") == 0){
			if(chdir("..") != 0)
				printf("bash: cd: %s: Arquivo ou diretório não encontrado\n", argumentos[1]);
		}else
		//trata a tentativa de ir para um diretório qualquer
			if(chdir(argumentos[1]) != 0)
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
		while(argumentos[i] != NULL){
			printf("arg[%d]: '%s'\n", i, argumentos[i]);
			i++;
		}
		//printf("\nfilho executando, pid: %d\n", getpid());
		execvp(argumentos[0], argumentos);
		//verifica erros
		verifica_erro(argumentos[0]);
	}

	//este bloco será executado pelo processo PAI
	if (pid > 0)
	{
		int retorno, status;
		//printf("\npai executando, pid: %d\n", getpid());

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
// função verifica erros colocado todos os erros da lista de erro da função execvp();
void verifica_erro(char *comando)
{   
	if (errno == ENOENT)
	{
		printf("%s: comando não é do pão\n", comando);
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
		printf("%s:ENFILE The system-wide limit on the total number of open files has been reached.\n",comando);
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