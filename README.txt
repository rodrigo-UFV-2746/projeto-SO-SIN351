/*
  UNIVERSIDADE FEDERAL DE VISOCA - CAMPUS RIO PARANAIBA
  PER 2020-4 PERIODO ESPECIAL REMOTO
  SIN 351 SISTEMAS OPERACIONAL
  PROFESSOR: RODRIGO MOREIRA
  Trabalho Prático I: Construindo um Interpretador Shell
  ALUNOS: 2746 RODRIGO DE OLIVEIRA COSTA
          6010 JOAO VITOR AZEVEDO     
*/

INSTRUÇÕES PARA EXECUCAO DO SIMULADOR DE SHELL:
1ª ABRA O TERMINAL NA PASTA QUE CONTEM OS ARQUIVOS copilador.c , makefile , readme.txt
2ª NO TERMINAL EXECUTE O SEGUITE COMANDO 'make'
3ª APOS EXECUTAR O COMANDO 'make' EXECUTE O COMANDO './shell'

OBS.:
    PARA COMANDO QUE UTILIZAREM O PIPE COMO NO EXEMPLO ' ls | cat readme.txt' O PIPE '|' DEVE SER TROCAO POR ',' COMO NO EXEPLO A SEGUIR ' ls , cat readme.txt'

TRABALHO DISPONIVEL NO LINK A SEGUIR:
    https://github.com/rodrigo-UFV-2746/projeto-SO-SIN351.git

COMANDOS TESTADOS:
cd ..
cd projeto-SO-SIN351
cat README.txt
nano README.txt
nano help
cat README.txt, grep 2746
,,cat README.txt
quit  *obs finaliza o programa
poweroff *obs o computador desliga e bom salvar as coisas antes desse comando.
mkdir pasta_teste
rm -r pasta_teste
ls
ls -l
date
uptime
ifconfig
ping 8.8.8.8
du 
netstat
lslogins , grep root
pwd
ls , wc -l
ls -l / , wc -l
