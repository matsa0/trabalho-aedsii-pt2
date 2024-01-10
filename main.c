#define _CRT_SECURE_NO_WARNINGS
#define MAX_PARTITIONS 120 // Defina o número máximo de partições .dat esperado na pasta
#define F 10 // Escolha o valor de F para intercalar as partições
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "shuffle.c"
#include "playlist.c"
#include "usuario.c"
#include "reviews.c"
#include "musicas.c"
#include "particoes.c"
#include "intercalacaoOtima.c"

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    FILE *log;
    FILE *arqPlaylist;
    const char *pastaPlaylist = "partitions_playlist";
    char **particoesArquivosPlaylist = (char **) malloc(MAX_PARTITIONS * sizeof(char *));
    int numeroParticoes = 0;

    //Abrindo o LOG
    if ((log = fopen("log.txt", "w")) == NULL) {
        printf("ERRO! Não foi possível abrir o arquivo LOG!\n");
        exit(1);
    } else {


        //PLAYLIST
        if ((arqPlaylist = fopen("playlist.dat", "w+b")) == NULL) {
            printf("ERRO! Não foi possível encontrar ou criar o arquivo PLAYLIST!\n");
            exit(1);
        }
        else {
            criarBaseDesordenadaPlaylist(arqPlaylist, 20000);

            clock_t startMergePlaylist = clock();
            int contMergeSort = mergeSortPlaylist(arqPlaylist, 0, tamanhoArquivoPlaylist(arqPlaylist) - 1);
            clock_t endMergePlaylist = clock();

            double timeMerge = ((double)(endMergePlaylist - startMergePlaylist)) / CLOCKS_PER_SEC;

            fprintf(log, "***Base de dados (20.000) de PLAYLIST***");
            fprintf(log, "\nTempo de ordenação com MERGE SORT > %.2f segundos.", timeMerge);
            fprintf(log, "\nNúmero de comparações com MERGE SORT > %d.", contMergeSort);
            fclose(arqPlaylist);
        }


        // Criar uma nova base desordenada para a classificação externa
        if ((arqPlaylist = fopen("playlist2.dat", "w+b")) == NULL) {
            printf("ERRO! Não foi possível encontrar ou criar o arquivo PLAYLIST2!\n");
            exit(1);
        }
        else {
            criarBaseDesordenadaPlaylist(arqPlaylist, 20000);
            fclose(arqPlaylist);
        }
        // Reabrir a nova base desordenada em modo de leitura binária para realizar a classificação externa na nova base desordenada
        if ((arqPlaylist = fopen("playlist2.dat", "r+b")) == NULL) {
            printf("ERRO! Não foi possível abrir o arquivo PLAYLIST2!\n");
            exit(1);
        } else {
            clock_t startClassificacaoExterna = clock();
            int countSelecaoNatural = 0;
            selecaoNaturalPlaylist(arqPlaylist, 120, &countSelecaoNatural);

            capturaParticoes(pastaPlaylist, particoesArquivosPlaylist, &numeroParticoes);

            int countIntercalacaoOtima = 0;
            if (numeroParticoes > 0) {
                intercalacaoOtima(particoesArquivosPlaylist, "partitions_playlist/playlist_intercalada.dat", numeroParticoes, &countIntercalacaoOtima);
                printf("Intercalacao PLAYLIST concluida com sucesso!\n");
            } else {
                printf("Nenhuma particao de PLAYLIST encontrada no diretorio.\n");
            }
            clock_t endClassificacaoExterna = clock();

            double timeClassificacao = ((double)(endClassificacaoExterna - startClassificacaoExterna)) / CLOCKS_PER_SEC;
            fprintf(log, "\n\nTempo de ordenação com SELEÇÃO NATURAL e INTERCALAÇÃO ÓTIMA > %.2f segundos.", timeClassificacao);
            fprintf(log, "\nNúmero de comparações com SELEÇÃO NATURAL + INTERCALAÇÃO ÓTIMA > %d.", countSelecaoNatural + countIntercalacaoOtima);

            // Liberar memória alocada para os nomes das partições
            for (int i = 0; i < numeroParticoes; i++) {
                free(particoesArquivosPlaylist[i]);
            }
            free(particoesArquivosPlaylist);

            fclose(arqPlaylist);
        }



        printf("\n\n*******************************************\n\n");
        numeroParticoes = 0;
        const char *pastaMusicas = "partitions_musicas";
        char **particoesArquivosMusicas = (char **) malloc(MAX_PARTITIONS * sizeof(char *));

        //MÚSICAS

        FILE* arqMusicas;
        if((arqMusicas = fopen("musicas.dat", "w+b")) == NULL) {
            printf("ERRO! Não foi possível encontrar ou criar o arquivo MUSICAS!\n");
            exit(1);
        }
        else {
            criarBaseDesordenadaMusicas(arqMusicas, 30000);
            clock_t startMergeMusicas = clock();
            int contMergeSort = mergeSortMusicas(arqMusicas, 0, tamanhoArquivoMusicas(arqMusicas) - 1);
            clock_t endMergeMusicas = clock();

            double timeMerge = ((double)(endMergeMusicas - startMergeMusicas)) / CLOCKS_PER_SEC;

            fprintf(log, "\n\n\n***Base de dados (30.000) de MUSICAS***");
            fprintf(log, "\nTempo de ordenação com MERGE SORT > %.2f segundos.", timeMerge);
            fprintf(log, "\nNúmero de comparações com MERGE SORT > %d.", contMergeSort);
            fclose(arqMusicas);
        }

        // Criar uma nova base desordenada para a classificação externa
        if ((arqMusicas = fopen("musicas2.dat", "w+b")) == NULL) {
            printf("ERRO! Não foi possível encontrar ou criar o arquivo MUSICAS2!\n");
            exit(1);
        }
        else {
            criarBaseDesordenadaMusicas(arqMusicas, 30000);
            fclose(arqMusicas);
        }
        // Reabrir a nova base desordenada em modo de leitura binária para realizar a classificação externa na nova base desordenada
        if ((arqMusicas = fopen("musicas2.dat", "r+b")) == NULL) {
            printf("ERRO! Não foi possível abrir o arquivo MUSICAS2!\n");
            exit(1);
        } else {
            clock_t startClassificacaoExterna = clock();
            int countSelecaoNatural = 0;
            selecaoNaturalMusicas(arqMusicas, 256, &countSelecaoNatural);

            capturaParticoes(pastaMusicas, particoesArquivosMusicas, &numeroParticoes);

            int countIntercalacaoOtima = 0;
            if (numeroParticoes > 0) {
                intercalacaoOtima(particoesArquivosMusicas, "partitions_musicas/musicas_intercalada.dat", numeroParticoes, &countIntercalacaoOtima);
                printf("Intercalacao MUSICAS concluida com sucesso!\n");
            } else {
                printf("Nenhuma particao de MUSICAS encontrada no diretorio.\n");
            }
            clock_t endClassificacaoExterna = clock();

            double timeClassificacao = ((double)(endClassificacaoExterna - startClassificacaoExterna)) / CLOCKS_PER_SEC;
            fprintf(log, "\n\nTempo de ordenação com SELEÇÃO NATURAL e INTERCALAÇÃO ÓTIMA > %.2f segundos.", timeClassificacao);
            fprintf(log, "\nNúmero de comparações com SELEÇÃO NATURAL + INTERCALAÇÃO ÓTIMA > %d.", countSelecaoNatural + countIntercalacaoOtima);

            // Liberar memória alocada para os nomes das partições
            for (int i = 0; i < numeroParticoes; i++) {
                free(particoesArquivosMusicas[i]);
            }
            free(particoesArquivosMusicas);

            fclose(arqMusicas);
        }




        printf("\n\n*******************************************\n\n");
        numeroParticoes = 0;
        const char *pastaUsuario = "partitions_usuario";
        char **particoesArquivosUsuario = (char **) malloc(MAX_PARTITIONS * sizeof(char *));

        //USUÁRIO

        FILE* arqUsuario;
        if((arqUsuario = fopen("usuario.dat", "w+b")) == NULL) {
            printf("ERRO! Não foi possível encontrar ou criar o arquivo PLAYLIST2!\n");
            exit(1);
        }
        else {
            criarBaseDesordenadaUsuario(arqUsuario, 50000);
            clock_t startMergeUsuario = clock();
            int contMergeSort = mergeSortUsuario(arqUsuario, 0, tamanhoArquivoUsuario(arqUsuario) - 1);
            clock_t endMergeUsuario = clock();

            double timeMerge = ((double)(endMergeUsuario - startMergeUsuario)) / CLOCKS_PER_SEC;

            fprintf(log, "\n\n\n***Base de dados (50.000) de USUARIO***");
            fprintf(log, "\nTempo de ordenação com MERGE SORT > %.2f segundos.", timeMerge);
            fprintf(log, "\nNúmero de comparações com MERGE SORT > %d.", contMergeSort);
            fclose(arqMusicas);
        }




        fflush(log);
        fclose(log);












    }

    return 0;
}



