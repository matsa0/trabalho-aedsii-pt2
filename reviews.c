#define _CRT_SECURE_NO_WARNINGS
#include "reviews.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include "reviews.h"
#include "shuffle.h"
#include "limits.h"

//Retorna o tamnanho da review em bytes
int tamanhoRegistroReviews() {
    return sizeof(int) +
           sizeof(char) * 200;
}

int tamanhoArquivoReviews(FILE * arq) {
    fseek(arq, 0, SEEK_END); //DESLOCA AT� O FINAL
    int tam = trunc(ftell(arq) / tamanhoRegistroReviews()); // ftel mede o deslocamento em bytes
    return tam;
}

//cria review
TReviews *reviews(int id, char *comentario) {
    TReviews *reviews = (TReviews *) malloc(sizeof(TReviews));
    if (reviews) memset(reviews, 0, sizeof(TReviews));
    reviews->id = id;
    strcpy(reviews->comentario, comentario);
    return reviews;
}

// Retorna um ponteiro para o review lido do arquivo
TReviews *leReviews(FILE* in) {
    TReviews *reviews = (TReviews *) malloc(sizeof(TReviews));
    if (0 >= fread(&reviews->id, sizeof(int), 1, in)) {
        free(reviews);
        return NULL;
    }
    fread(reviews->comentario, sizeof(char), sizeof(reviews->comentario), in);
    return reviews;
}

//salva playlist
void salvaReviews(TReviews *reviews, FILE *out) {
    fwrite(&reviews->id, sizeof(int), 1, out);
    fwrite(reviews->comentario, sizeof(char), sizeof(reviews->comentario), out);
}

// Cria a base de dados desordenada
void criarBaseDesordenadaReviews(FILE *out, int tam) {
    int vet[tam];
    TReviews *review;

    for(int i=0;i<tam;i++)
        vet[i] = i+1;

    shuffle(vet, tam, 0);

    printf("\n\nGerando a base de dados DESORDENADA de REVIEWS...\n");

    for (int i=0;i<tam;i++){
        review = reviews(vet[i], "“Locked Out of Heaven” finds Mars borrowing heavily from the ’80s, thanks...");
        salvaReviews(review, out);
        free(review);
    }

}

// Cria a base de dados ordenada
void criarBaseOrdenadaReviews(FILE *out, int tam) {
    int vet[tam];
    TReviews *review;

    for(int i=0;i<tam;i++)
        vet[i] = i+1;

    printf("\n\nGerando a base de dados ORDENADA de REVIEWS...\n");

    for (int i=0;i<tam;i++){
        review = reviews(vet[i], "Musica com uma linda letra e ritmo cativante!");
        salvaReviews(review, out);
        free(review);
    }
}

// Imprime review
void imprimeReviews(TReviews *reviews) {
    printf("**********************************************");
    printf("\nID Review > %d", reviews->id);
    printf("\nComentario > %s", reviews->comentario);
    printf("\n**********************************************");
}

// Imprime a base de dados
void imprimirBaseReviews(FILE *out) {
    printf("\nImprimindo a base de dados REVIEWS...\n");

    rewind(out);
    TReviews *reviews;

    while ((reviews = leReviews(out)) != NULL) {
        imprimeReviews(reviews);
    }
    free(reviews);

}

//escrever a busca sequencial no log
void logBuscaSequencialReviews(FILE* out, int count, clock_t start_time) {
    sleep(1);
    clock_t end_time = clock();
    fprintf(out, "\n------------------------------");
    fprintf(out, "\nBusca sequencial REVIEWS");
    fprintf(out, "\nNumero de comparações > %d", count);
    fprintf(out, "\nTempo de Busca > %.2f segundos", (double)(end_time - start_time) / CLOCKS_PER_SEC);
    fprintf(out, "\n------------------------------");
    fprintf(out, "\n");
}

//busca sequencial sobre a base
TReviews *buscaSequencialReviews(int chave, FILE *in, FILE* out) {
    TReviews *reviews;
    int achou = 0;
    rewind(in);
    int count = 0;
    clock_t start_time = clock();

    while ((reviews = leReviews(in)) != NULL){
        if(reviews->id == chave){
            achou = 1;
            break;
        }
        count++;
    }

    if(achou == 1) {
        logBuscaSequencialReviews(out, count, start_time);
        return reviews;
    }
    else {
        printf("ERRO! REVIEW nao encontrado.");
        logBuscaSequencialReviews(out, count, start_time);
    }


    free(reviews);
    return NULL;
}

//escrever a busca binária no log
void logBuscaBinariaReviews(FILE* out, int count, clock_t start_time) {
    sleep(1);
    clock_t end_time = clock();
    fprintf(out, "\n------------------------------");
    fprintf(out, "\nBusca binária REVIEWS");
    fprintf(out, "\nNumero de comparações > %d", count);
    fprintf(out, "\nTempo de Busca > %.2f segundos", (double)(end_time - start_time) / CLOCKS_PER_SEC);
    fprintf(out, "\n------------------------------");
    fprintf(out, "\n");
}

//busca binária sobre a base
TReviews *buscaBinariaReviews(int chave, FILE *in, FILE *out, int inicio, int fim) {
    TReviews *reviews = NULL;
    int cod = -1;
    int count = 0;
    clock_t start_time;
    start_time = clock();

    while (inicio <= fim && cod != chave) {
        int meio = trunc((inicio + fim) / 2);
        //printf("\nInicio: %d; Fim: %d; Meio: %d\n", inicio, fim, meio);
        fseek(in, (meio -1) * tamanhoRegistroReviews(), SEEK_SET);
        reviews = leReviews(in);
        cod = &reviews->id;

        if (reviews) {
            if (cod > chave) {
                fim = meio - 1;
            } else {
                inicio = meio + 1;
            }
        }

        count++;
    }

    if (cod == chave) {
        logBuscaBinariaReviews(out, count, start_time);
        return reviews;
    }

    logBuscaBinariaReviews(out, count, start_time);

    return NULL;
}

int countComparacoes = 0;
//Mesclar duas partições ordenadas
void mergeReviews(FILE *arq, int inicio, int meio, int fim) {
    int i, j, k;
    int n1 = meio - inicio + 1;
    int n2 = fim - meio;

    // Partições temporárias
    TReviews *esq = (TReviews *)malloc(n1 * sizeof(TReviews));
    TReviews *dir = (TReviews *)malloc(n2 * sizeof(TReviews));

    // Copia os dados para as partições temporárias
    fseek(arq, inicio * tamanhoRegistroReviews(), SEEK_SET);
    for (i = 0; i < n1; i++) {
        esq[i] = *leReviews(arq);
    }

    fseek(arq, (meio + 1) * tamanhoRegistroReviews(), SEEK_SET);
    for (j = 0; j < n2; j++) {
        dir[j] = *leReviews(arq);
    }

    // Mescla as partições temporárias em ordem no arquivo
    i = 0;
    j = 0;
    k = inicio;

    while (i < n1 && j < n2) {
        if (esq[i].id <= dir[j].id) {
            salvaReviews(&esq[i], arq);
            i++;
        } else {
            salvaReviews(&dir[j], arq);
            j++;
        }
        k++;
        countComparacoes++;
    }

    // Copia os elementos restantes, se houver, de esq[] para o arquivo
    while (i < n1) {
        salvaReviews(&esq[i], arq);
        i++;
        k++;
    }

    // Copia os elementos restantes, se houver, de dir[] para o arquivo
    while (j < n2) {
        salvaReviews(&dir[j], arq);
        j++;
        k++;
    }

    free(esq);
    free(dir);
}

//Ordenação por MergeSort
int mergeSortReviews(FILE *arq, int inicio, int fim) {
    if (inicio < fim) {
        int meio = inicio + (fim - inicio) / 2;

        mergeSortReviews(arq, inicio, meio);
        mergeSortReviews(arq, meio + 1, fim);

        mergeReviews(arq, inicio, meio, fim);
    }
    return countComparacoes;
}

//Seleção Natural para partições maiores
int selecaoNaturalRevoiews(FILE *arqEntrada, int M, int *numComparacoes) {
    TUsuario *reservatorio = (TUsuario *)malloc(M * sizeof(TUsuario)); //reservatório do tamanho da entrada
    int posicaoReservatorio = 0;
    int numParticao = 1;

    //verifica se há alguma partição no diretório, se tiver, ele remove
    for (int i = 1; i <= MAX_PARTITIONS; i++) {
        char nomeParticaoExistente[20];
        sprintf(nomeParticaoExistente, "partitions_reviews/particao_%d.dat", i);
        remove(nomeParticaoExistente);
    }

    //(Passo 1) Leitura inicial dos primeiros M registros do arquivo para o reservatório
    for (int i = 0; i < M; i++) {
        if (fread(&reservatorio[i], sizeof(TUsuario), 1, arqEntrada) != 1) {
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
        sprintf(nomeParticao, "partitions_reviews/particao_%d.dat", numParticao);
        FILE *out = fopen(nomeParticao, "w+b");
        fwrite(&reservatorio[menorChave], sizeof(TUsuario), 1, out);
        fclose(out);

        //(Passo 4) Substituir, no reservatório, o registro r pelo próximo registro do arqEntrada
        if (fread(&reservatorio[menorChave], sizeof(TUsuario), 1, arqEntrada) != 1) {
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
            if (fread(&reservatorio[i], sizeof(TUsuario), 1, arqEntrada) != 1) {
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