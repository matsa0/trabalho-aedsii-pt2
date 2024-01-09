#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "musicas.h"
#include "shuffle.h"
#include "reviews.h"
#include "limits.h"

//Retorna o tamnanho da playlist em bytes
int tamanhoRegistroMusicas() {
    return sizeof(int) +
           sizeof(char) * 50 +
           sizeof(char) * 20 +
           sizeof(char) * 30;
}

int tamanhoArquivoMusicas(FILE * arq) {
    fseek(arq, 0, SEEK_END); //DESLOCA AT� O FINAL
    int tam = trunc(ftell(arq) / tamanhoRegistroMusicas()); // ftel mede o deslocamento em bytes
    return tam;
}

//cria musica
TMusicas *musicas(int id, char *titulo, char *estilo, char *artista) {
    TMusicas *musicas = (TMusicas *) malloc(sizeof(TMusicas));
    if (musicas) memset(musicas, 0, sizeof(TMusicas));
    musicas->id = id;
    strcpy(musicas->titulo, titulo);
    strcpy(musicas->estilo, estilo);
    strcpy(musicas->artista, artista);
    return musicas;
}

// Retorna um ponteiro para a musica lida do arquivo
TMusicas *leMusicas(FILE* in) {
    TMusicas *musicas = (TMusicas *) malloc(sizeof(TMusicas));
    if (0 >= fread(&musicas->id, sizeof(int), 1, in)) {
        free(musicas);
        return NULL;
    }
    fread(musicas->titulo, sizeof(char), sizeof(musicas->titulo), in);
    fread(musicas->estilo, sizeof(char), sizeof(musicas->estilo), in);
    fread(musicas->artista, sizeof(char), sizeof(musicas->artista), in);
    return musicas;
}

//salva musicas
void salvaMusicas(TMusicas *musicas, FILE *out) {
    fwrite(&musicas->id, sizeof(int), 1, out);
    fwrite(musicas->titulo, sizeof(char), sizeof(musicas->titulo), out);
    fwrite(musicas->estilo, sizeof(char), sizeof(musicas->estilo), out);
    fwrite(musicas->artista, sizeof(char), sizeof(musicas->artista), out);
}


// Cria a base de dados desordenada
void criarBaseDesordenadaMusicas(FILE *out, int tam) {
    int vet[tam];
    TMusicas *music;

    for(int i=0;i<tam;i++)
        vet[i] = i+1;

    shuffle(vet, tam, 0);

    printf("\n\nGerando a base de dados DESORDENADA de MUSICAS...\n");

    for (int i=0;i<tam;i++){
        music = musicas(vet[i], "Locked out of Heaven", "Pop rock", "Bruno Mars");
        salvaMusicas(music, out);
        free(music);
    }
}

// Cria a base de dados ordenada
void criarBaseOrdenadaMusicas(FILE* out, int tam) {
    int vet[tam];
    TMusicas *music;

    for(int i=0;i<tam;i++)
        vet[i] = i+1;

    printf("\n\nGerando a base de dados ORDENADA de MUSICAS...\n");

    for (int i=0;i<tam;i++){
        music = musicas(vet[i], "Ainda gosto dela", "Pop rock", "Skank");
        salvaMusicas(music, out);
        free(music);
    }
}

// Imprime musicas
void imprimeMusicas(TMusicas *musicas) {
    printf("**********************************************");
    printf("\nID Musica > %d", musicas->id);
    printf("\nTitulo > %s", musicas->titulo);
    printf("\nEstilo > %s", musicas->estilo);
    printf("\nArtista > %s", musicas->artista);
    printf("\n**********************************************");
}


// Imprime a base de dados
void imprimirBaseMusicas(FILE *out) {
    printf("\nImprimindo a base de dados MUSICAS...\n");

    rewind(out);
    TMusicas *musicas;

    while ((musicas = leMusicas(out)) != NULL)
        imprimeMusicas(musicas);

    free(musicas);
}

//escrever a busca sequencial no log
void logBuscaSequencialMusicas(FILE* out, int count, clock_t start_time) {
    //sleep(1);
    clock_t end_time = clock();
    fprintf(out, "\n------------------------------");
    fprintf(out, "\n*Busca sequencial MUSICAS");
    fprintf(out, "\nNumero de comparações > %d", count);
    fprintf(out, "\nTempo de Busca > %.2f segundos", (double)(end_time - start_time) / CLOCKS_PER_SEC);
    fprintf(out, "\n------------------------------");
    fprintf(out, "\n");
}

//busca sequencial sobre a base
TMusicas *buscaSequencialMusicas(int chave, FILE *in, FILE* out) {
    TMusicas *musicas;
    int achou = 0;
    rewind(in);
    int count = 0;
    clock_t start_time = clock();

    while ((musicas = leMusicas(in)) != NULL){
        if(musicas->id == chave){
            achou = 1;
            break;
        }
        count++;
    }

    if(achou == 1) {
        logBuscaSequencialMusicas(out, count, start_time);
        return musicas;
    }
    else {
        printf("ERRO! MUSICA nao encontrada.");
        logBuscaSequencialMusicas(out, count, start_time);
    }


    free(musicas);
    return NULL;
}

//escrever a busca binária no log
void logBuscaBinariaMusicas(FILE* out, int count, clock_t start_time) {
    //sleep(1);
    clock_t end_time = clock();
    fprintf(out, "\n------------------------------");
    fprintf(out, "\n*Busca binária MUSICAS");
    fprintf(out, "\nNumero de comparações > %d", count);
    fprintf(out, "\nTempo de Busca > %.2f segundos", (double)(end_time - start_time) / CLOCKS_PER_SEC);
    fprintf(out, "\n------------------------------");
    fprintf(out, "\n");
}

//busca binária sobre a base
TMusicas *buscaBinariaMusicas(int chave, FILE *in, FILE *out, int inicio, int fim) {
    TMusicas *musicas = NULL;
    int cod = -1;
    int count = 0;
    clock_t start_time;
    start_time = clock();

    while (inicio <= fim && cod != chave) {
        int meio = trunc((inicio + fim) / 2);
        //printf("\nInicio: %d; Fim: %d; Meio: %d\n", inicio, fim, meio);
        fseek(in, (meio -1) * tamanhoRegistroMusicas(), SEEK_SET);
        musicas = leMusicas(in);
        cod = &musicas->id;

        if (musicas) {
            if (cod > chave) {
                fim = meio - 1;
            } else {
                inicio = meio + 1;
            }
        }

        count++;
    }

    if (cod == chave) {
        logBuscaBinariaMusicas(out, count, start_time);
        return musicas;
    }

    logBuscaBinariaMusicas(out, count, start_time);

    return NULL;
}

int count = 0;
//Mesclar duas partições ordenadas
void mergeMusicas(FILE *arq, int inicio, int meio, int fim) {
    int i, j, k;
    int n1 = meio - inicio + 1;
    int n2 = fim - meio;

    // Partições temporárias
    TMusicas *esq = (TMusicas *)malloc(n1 * sizeof(TMusicas));
    TMusicas *dir = (TMusicas *)malloc(n2 * sizeof(TMusicas));

    // Copia os dados para as partições temporárias
    fseek(arq, inicio * tamanhoRegistroMusicas(), SEEK_SET);
    for (i = 0; i < n1; i++) {
        esq[i] = *leMusicas(arq);
    }

    fseek(arq, (meio + 1) * tamanhoRegistroMusicas(), SEEK_SET);
    for (j = 0; j < n2; j++) {
        dir[j] = *leMusicas(arq);
    }

    // Mescla as partições temporárias em ordem no arquivo
    i = 0;
    j = 0;
    k = inicio;

    while (i < n1 && j < n2) {
        if (esq[i].id <= dir[j].id) {
            salvaMusicas(&esq[i], arq);
            i++;
        } else {
            salvaMusicas(&dir[j], arq);
            j++;
        }
        k++;
        count++;
    }

    // Copia os elementos restantes, se houver, de esq[] para o arquivo
    while (i < n1) {
        salvaMusicas(&esq[i], arq);
        i++;
        k++;
    }

    // Copia os elementos restantes, se houver, de dir[] para o arquivo
    while (j < n2) {
        salvaMusicas(&dir[j], arq);
        j++;
        k++;
    }

    free(esq);
    free(dir);
}

//Ordenação por MergeSort
int mergeSortMusicas(FILE *arq, int inicio, int fim) {
    if (inicio < fim) {
        int meio = inicio + (fim - inicio) / 2;

        mergeSortMusicas(arq, inicio, meio);
        mergeSortMusicas(arq, meio + 1, fim);

        mergeMusicas(arq, inicio, meio, fim);
    }
    return count;
}

//Seleção Natural para partições maiores
int selecaoNaturalMusicas(FILE *arqEntrada, int M, int *numComparacoes) {
    TMusicas *reservatorio = (TMusicas *)malloc(M * sizeof(TMusicas)); //reservatório do tamanho da entrada
    int posicaoReservatorio = 0;
    int numParticao = 1;

    //(Passo 1) Leitura inicial dos primeiros M registros do arquivo para o reservatório
    for (int i = 0; i < M; i++) {
        if (fread(&reservatorio[i], sizeof(TMusicas), 1, arqEntrada) != 1) {
            break;
        }
    }

    while (1) { //Enquanto houver registros no arqEntrada
        //(Passo 2) Encontrar o registro com a menor chave no reservatório
        int menorChave = 0;
        for (int i = 1; i < M; i++) {
            (*numComparacoes)++;
            if (reservatorio[i].id < reservatorio[menorChave].id) {
                menorChave = i;
            }
        }


        //(Passo 3) Abre a partição de saída, grava o registro ordenado e fecha
        char nomeParticao[20];
        sprintf(nomeParticao, "partitions_musicas/particao_%d.dat", numParticao);
        FILE *out = fopen(nomeParticao, "w+b");
        fwrite(&reservatorio[menorChave], sizeof(TMusicas), 1, out);
        fclose(out);

        //(Passo 4) Substituir, no reservatório, o registro r pelo próximo registro do arqEntrada
        if (fread(&reservatorio[menorChave], sizeof(TMusicas), 1, arqEntrada) != 1) {
            break;
        }

        //(Passo 5) Se a chave do último registro lido for menor que a chave recém gravada na partição, colocar no reservatório
        if (reservatorio[menorChave].id < reservatorio[0].id) {
            posicaoReservatorio = 0;
            continue;
        }

        //(Passo 6) Copiar registros restantes do reservatório para o array principal em memória
        int i;
        for (i = 0; i < M; i++) {
            if (fread(&reservatorio[i], sizeof(TMusicas), 1, arqEntrada) != 1) {
                break;
            }
        }

        //(Passo 7) Atualiza o restante do reservatório com novos registros do arquivo de entrada
        posicaoReservatorio = i;
        numParticao++;

        // Se o reservatório está cheio, grava a próxima partição de saída
        if (i == M) {
            i = 0; // Reinicia o índice do reservatório
        }
    }
    free(reservatorio);
    return *numComparacoes;
}



/*
//Adiciona review para a música
void adicionaReview(TMusicas* musica, TReviews* review) {
    musica->review = review;
}
*/