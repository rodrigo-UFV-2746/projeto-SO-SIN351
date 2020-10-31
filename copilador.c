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
		fflush(stdin);
		gets(linha);
		
		if (strcmp(linha, "exit") == 0)
			exit(EXIT_SUCCESS);
		
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

void verifica_erro(char *comando)
{
	if (errno == EEXIST){
		printf("%s: comando existe\n", comando);
	}
	if (errno == ENOENT)
	{
		printf("%s: diretório ou comando inválido\n", comando);
	}
	exit(EXIT_FAILURE);
}

void executa_comando(char **argumentos)
{
	pid_t pid;

	//se fork() não conseguir criar um processo
	if ((pid = fork()) < 0)
	{
		perror("Erro em fork");
		exit(EXIT_FAILURE);
	}

	//este bloco será executado pelo processo FILHO
	if (pid == 0)
	{
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

/*
{		E2BIG  The total number of bytes in the environment (envp) and argument list (argv) is too
              large.

       EACCES Search  permission  is  denied on a component of the path prefix of pathname or the
              name of a script interpreter.  (See also path_resolution(7).)

       EACCES The file or a script interpreter is not a regular file.

       EACCES Execute permission is denied for the file or a script or ELF interpreter.

       EACCES The filesystem is mounted noexec.

       EAGAIN (since Linux 3.1)
              Having changed its real UID using one of the set*uid() calls, the caller was—and is
              now  still—above  its  RLIMIT_NPROC  resource limit (see setrlimit(2)).  For a more
              detailed explanation of this error, see NOTES.

       EFAULT pathname or one of the pointers in the vectors argv or  envp  points  outside  your
              accessible address space.

       EINVAL An  ELF  executable  had  more than one PT_INTERP segment (i.e., tried to name more
              than one interpreter).

       EIO    An I/O error occurred.

       EISDIR An ELF interpreter was a directory.

       ELIBBAD
              An ELF interpreter was not in a recognized format.

       ELOOP  Too many symbolic links were encountered in resolving pathname or  the  name  of  a
              script or ELF interpreter.

       ELOOP  The maximum recursion limit was reached during recursive script interpretation (see
              "Interpreter scripts", above).  Before Linux 3.8, the error produced for this  case
              was ENOEXEC.

       EMFILE The per-process limit on the number of open file descriptors has been reached.

       ENAMETOOLONG
              pathname is too long.

       ENFILE The system-wide limit on the total number of open files has been reached.

       ENOENT The file pathname or a script or ELF interpreter does not exist.

       ENOEXEC
              An  executable is not in a recognized format, is for the wrong architecture, or has
              some other format error that means it cannot be executed.

       ENOMEM Insufficient kernel memory was available.

       ENOTDIR
              A component of the path prefix of pathname or a script or ELF interpreter is not  a
              directory.

       EPERM  The  filesystem  is mounted nosuid, the user is not the superuser, and the file has
              the set-user-ID or set-group-ID bit set.

       EPERM  The process is being traced, the user is not the superuser and  the  file  has  the
              set-user-ID or set-group-ID bit set.

       EPERM  A  "capability-dumb"  applications  would  not  obtain  the  full  set of permitted
              capabilities granted by the executable file.  See capabilities(7).

       ETXTBSY
              The specified executable was open for writing by one or more processes.}
*/