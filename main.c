/*
Trabalho SO 2
Arion denovaro
professor: Sergio Marques
incio: 25/03/2020
fim :  04/04/2020
---------------
Implementar um programa que crie processos filhos,
que realizarão o processamento de arquivos
(contagem do número de linhas de cada arquivo)
de forma independente, e atualizarão o resultado na
memória compartilhada do processo pai que consolidará
e imprimirá o número de linhas total processado
pelos processos filhos.


*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include<sys/stat.h>


/* constantes */
#define ABERTURA "TRABALHO conta Numero de linhas varios arquivos"
#define PROC_FILHO "Processo filho criado \n"
#define PROC_PAI "Processo pai\n"
#define PROC_ERRO "ERRO!! ao criar processo\n"
#define ARQ_CAMINHO "arq"
#define EXTENSAO  ".txt"
#define N_PROCES_FILHO 5

//cabeçario da func
short int conta_linhas_arq(char *nome);

/*
funções para compartilhar memoria
shmget
shmat
shmdt
*/

int main(int argc, char *argv[])
{
/* numero de processos filhos */
    pid_t procesos[N_PROCES_FILHO];
// segemtno que sera alocado
    short int *quant_linhas_arq=NULL;
// idenficidador do segmento
    int id_segmento = 0;
//nome do arquivo que será concatenado   
    char nome_arquivo[100];
 // irá concatenar a string do numero do arquivo
    char ch[5];


   write(STDOUT_FILENO,ABERTURA,strlen(ABERTURA));

 /* Alloca memoria compartilhada/ leitura escrita*/
   /*indentifica o segmento primeiro para reservar espaço*/
  id_segmento = shmget(IPC_PRIVATE,
                       sizeof (short int)*N_PROCES_FILHO,
                       S_IRUSR|S_IWUSR);

/*alloca a memoria do segmento*/
  quant_linhas_arq = (short int *) shmat(id_segmento,NULL,NULL);


/*chama os processos filhos*/
   for(int i =0; i< N_PROCES_FILHO; i++)
   {

//cria procesos 2^
    procesos[i] = fork();

 // processo filho criado
    if(procesos[i] > 0)
    { write(STDOUT_FILENO,PROC_FILHO,strlen(PROC_FILHO)); }

 //processo pai
    else if (procesos[i] == 0)
    { write(STDOUT_FILENO,PROC_PAI,strlen(PROC_PAI)); }

  // erro ao criar processo
    else if (procesos[i] < 0)
    { write(STDOUT_FILENO,PROC_ERRO,strlen(PROC_ERRO)); }

    printf("processo id %d\n",getpid());
    printf("processos memoria %d \n",procesos[i]);

 /*contaena o final do arquivo para montar a string*/

    sprintf(ch,"%d",i);
    strcat(nome_arquivo,ARQ_CAMINHO);
    strcat(nome_arquivo,ch);
    strcat(nome_arquivo,EXTENSAO);
    strcat(nome_arquivo,"\0");
/*mostra como ficou a string*/
   printf("%s \n",nome_arquivo);

/*chama a função que conta as linhas escreve na memoria compara
atualiza memoria compartilhada com os dados*/
   quant_linhas_arq[i] = conta_linhas_arq(nome_arquivo);

  /*limpa a string*/
    memset(nome_arquivo,0,strlen(nome_arquivo));
   }

/*espera os processos filhos*/
  for(int i =0; i< N_PROCES_FILHO; i++)
  {
 /*espera os processos filhos*/
    wait(NULL);

    printf("processo id %d\n",getpid());
    printf("processos em memoria %d \n",procesos[i]);
  }

/*imprime quant de linhas dos arquivos*/
   for(int i =0; i< N_PROCES_FILHO; i++)
   {
     printf("no arq%d =  %d\n",i,quant_linhas_arq[i]);
   }

/* Desalloca memoria compartilhada*/
  shmdt(quant_linhas_arq);
  quant_linhas_arq =NULL;

 return EXIT_SUCCESS;
}



/*
 * ----------------------------------------
 * Função que conta numero de linhas arquivo
*/
short int conta_linhas_arq(char *nome)
{
  int num_linhas = 0;
  FILE *arq = NULL;
  char c;

  //abre o arquivo
  arq = fopen(nome,"r");

  // ocorreu erro ao abrir arquivo
  if(arq == NULL)
  {
   perror("Erro ao abrir o arquivo\n");
   return -1;
  }

  //percorre o ate o fim do arquivo
  while (! feof(arq))
  {
   fread (&c, sizeof(char), 1, arq);
   // se encontrar linha nova, add ,
   if(c =='\n')
   { num_linhas++; }
  }
  fclose(arq);

  //retorna nlinhas
  return(num_linhas);
}
