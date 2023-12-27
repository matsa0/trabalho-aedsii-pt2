#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "shuffle.c"
#include "playlist.c"
#include "usuario.c"
#include "reviews.c"
#include "musicas.c"

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main()
{

    char imprimir;
    FILE *log;
    FILE *arqPlaylist;

    if((log = fopen("log.txt", "w")) == NULL) {
        printf("ERRO! Nao foi possivel abrir o arquibo LOG!\n");
        exit(1);
    }
    else {

        //BASES DESORDENADAS E ORDENADAS DE PLAYLIST
        if((arqPlaylist = fopen("playlist.dat", "w+b")) == NULL) {
            printf("ERRO! Nao foi possivel encontrar o arquivo PLAYLIST!");
            exit(1);
        }
        else {
            //BASE DE DADOS DESORDENADA
            criarBaseDesordenadaPlaylist(arqPlaylist, 1000);
            printf("Base DESORDENADA(1.000) de PLAYLIST criada, deseja imprimir? (S ou N): ");
            scanf("%c", &imprimir);

            if (imprimir == 's' || imprimir == 'S') {
                imprimirBasePlaylist(arqPlaylist);
            }

            clock_t startMergePlaylist = clock();
            mergeSort(arqPlaylist, 0, tamanhoArquivoPlaylist(arqPlaylist)-1); //Ao pasar -1, eu indico que desejo ordenar a partir do índice zero até o índice correspondente ao último elemento do seu conjunto de dados.
            clock_t endMergePlaylist = clock();

            double timeMerge = ((double)(endMergePlaylist - startMergePlaylist)) / CLOCKS_PER_SEC;

            fprintf(log, "***Base de dados (1.000) de PLAYLIST***");
            fprintf(log, "\nTempo de ordenação com MERGE SORT > %.2f segundos.", timeMerge);

            printf("Tempo de ordenacao com MERGE SORT > %.2f", timeMerge);

            fclose(arqPlaylist);
        }

        limparBufferEntrada();


        fclose(log);
    }

    return 0;
}


