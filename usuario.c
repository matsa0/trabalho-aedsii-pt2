#define _CRT_SECURE_NO_WARNINGS
#include "usuario.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include "usuario.h"
#include "shuffle.h"

//Retorna o tamnanho do usuario em bytes
int tamanhoRegistroUsuario() {
    return sizeof(int) +
           sizeof(char) * 30 +
           sizeof(char) * 20 +
           sizeof(char) * 30;
}

int tamanhoArquivoUsuario(FILE* arq) {
    fseek(arq, 0, SEEK_END);
    int tam = trunc(ftell(arq) / tamanhoRegistroUsuario()); // ftel mede o deslocamento em bytes
    return tam;
}

TUsuario *usuario(int id, char *senha, char *nome) {
    TUsuario *usuario = (TUsuario *)malloc(sizeof(TUsuario));
    if (usuario) memset(usuario, 0, sizeof(TUsuario));

    usuario->id = id;
    sprintf(usuario->user, "user%d", id);
    strcpy(usuario->senha, senha);
    strcpy(usuario->nome, nome);
    return usuario;
}

TUsuario *leUsuario(FILE* in) {
    TUsuario *usuario = (TUsuario *)malloc(sizeof(TUsuario));
    if (0 >= fread(&usuario->id, sizeof(int), 1, in)) {
        free(usuario);
        return NULL;
    }
    fread(usuario->user, sizeof(char), sizeof(usuario->user), in);
    fread(usuario->senha, sizeof(char), sizeof(usuario->senha), in);
    fread(usuario->nome, sizeof(char), sizeof(usuario->nome), in);

    return usuario;
}

void salvaUsuario(TUsuario *usuario, FILE *out) {
    fwrite(&usuario->id, sizeof(int), 1, out);
    fwrite(usuario->user, sizeof(char), sizeof(usuario->user), out);
    fwrite(usuario->senha, sizeof(char), sizeof(usuario->senha), out);
    fwrite(usuario->nome, sizeof(char), sizeof(usuario->nome), out);
}

void criarBaseDesordenadaUsuario(FILE *out, int tam) {
    int vet[tam];
    TUsuario *user;

    for(int i=0;i<tam;i++)
        vet[i] = i+1;

    shuffle(vet, tam, 0);

    printf("\n\nGerando a base de dados DESORDENADA de USUARIOS...\n");

    for (int i=0;i<tam;i++){
        user = usuario(vet[i], "12345678", "Jubileu");
        salvaUsuario(user, out);
        free(user);
    }
}

// Cria a base de dados ordenada
void criarBaseOrdenadaUsuario(FILE *out, int tam) {
    int vet[tam];
    TUsuario *user;

    for(int i=0;i<tam;i++)
        vet[i] = i+1;

    printf("\n\nGerando a base de dados ORDENADA de USUARIO...\n");

    for (int i=0;i<tam;i++){
        user = usuario(vet[i], "12345678", "Jubileu");
        salvaUsuario(user, out);
        free(user);
    }
}

void imprimeUsuario(TUsuario* usuario) {
    printf("**********************************************");
    printf("\nUsername: %s", usuario->user);
    printf("\nSenha: %s", usuario->senha);
    printf("\nNome: %s", usuario->nome);
    printf("\n**********************************************");
}

void imprimirBaseUsuario(FILE *out) {
    printf("\nImprimindo a base de dados USUARIOS...\n");

    rewind(out);
    TUsuario *usuario;

    while ((usuario = leUsuario(out)) != NULL) {
        imprimeUsuario(usuario);
    }
    free(usuario);
}

//escrever a busca sequencial no log
void logBuscaSequencialUsuario(FILE* out, int count, clock_t start_time) {
    sleep(1);
    clock_t end_time = clock();
    fprintf(out, "\n------------------------------");
    fprintf(out, "\nBusca sequencial USUARIO");
    fprintf(out, "\nNumero de comparações > %d", count);
    fprintf(out, "\nTempo de Busca > %.2f segundos", (double)(end_time - start_time) / CLOCKS_PER_SEC);
    fprintf(out, "\n------------------------------");
    fprintf(out, "\n");
}

//busca sequencial sobre a base
TUsuario *buscaSequencialUsuario(int chave, FILE *in, FILE* out) {
    TUsuario *usuario;
    int achou = 0;
    rewind(in);
    int count = 0;
    clock_t start_time = clock();

    while ((usuario = leUsuario(in)) != NULL){
        if(usuario->id == chave){
            achou = 1;
            break;
        }
        count++;
    }

    if(achou == 1) {
        logBuscaSequencialUsuario(out, count, start_time);
        return usuario;
    }
    else {
        printf("ERRO! USUARIO nao encontrado.");
        logBuscaSequencialUsuario(out, count, start_time);
    }
    free(usuario);
    return NULL;
}

//escrever a busca binária no log
void logBuscaBinariaUsuario(FILE* out, int count, clock_t start_time) {
    //sleep(1);
    clock_t end_time = clock();
    fprintf(out, "\n------------------------------");
    fprintf(out, "\nBusca binária USUARIO");
    fprintf(out, "\nNumero de comparações > %d", count);
    fprintf(out, "\nTempo de Busca > %.2f segundos", (double)(end_time - start_time) / CLOCKS_PER_SEC);
    fprintf(out, "\n------------------------------");
    fprintf(out, "\n");
}

//busca binária sobre a base
TUsuario *buscaBinariaUsuario(int chave, FILE *in, FILE *out, int inicio, int fim) {
    TUsuario *usuario = NULL;
    int cod = -1;
    int count = 0;
    clock_t start_time;
    start_time = clock();

    while (inicio <= fim && cod != chave) {
        int meio = trunc((inicio + fim) / 2);
        //printf("\nInicio: %d; Fim: %d; Meio: %d\n", inicio, fim, meio);
        fseek(in, (meio -1) * tamanhoRegistroUsuario(), SEEK_SET);
        usuario = leUsuario(in);
        cod = usuario->id;

        if (usuario) {
            if (cod > chave) {
                fim = meio - 1;
            } else {
                inicio = meio + 1;
            }
        }

        count++;
    }

    if (cod == chave) {
        logBuscaBinariaUsuario(out, count, start_time);
        return usuario;
    }

    logBuscaBinariaUsuario(out, count, start_time);

    return NULL;
}



int xcont = 0;
//Mesclar duas partições ordenadas
void mergeUsuario(FILE *arq, int inicio, int meio, int fim) {
    int i, j, k;
    int n1 = meio - inicio + 1;
    int n2 = fim - meio;

    // Partições temporárias
    TUsuario *esq = (TUsuario *)malloc(n1 * sizeof(TUsuario));
    TUsuario *dir = (TUsuario *)malloc(n2 * sizeof(TUsuario));

    // Copia os dados para as partições temporárias
    fseek(arq, inicio * tamanhoRegistroUsuario(), SEEK_SET);
    for (i = 0; i < n1; i++) {
        esq[i] = *leUsuario(arq);
    }

    fseek(arq, (meio + 1) * tamanhoRegistroUsuario(), SEEK_SET);
    for (j = 0; j < n2; j++) {
        dir[j] = *leUsuario(arq);
    }

    // Mescla as partições temporárias em ordem no arquivo
    i = 0;
    j = 0;
    k = inicio;

    while (i < n1 && j < n2) {
        if (esq[i].id <= dir[j].id) {
            salvaUsuario(&esq[i], arq);
            i++;
        } else {
            salvaUsuario(&dir[j], arq);
            j++;
        }
        k++;
        xcont++;
    }

    // Copia os elementos restantes, se houver, de esq[] para o arquivo
    while (i < n1) {
        salvaUsuario(&esq[i], arq);
        i++;
        k++;
    }

    // Copia os elementos restantes, se houver, de dir[] para o arquivo
    while (j < n2) {
        salvaUsuario(&dir[j], arq);
        j++;
        k++;
    }

    free(esq);
    free(dir);
}

//Ordenação por MergeSort
int mergeSortUsuario(FILE *arq, int inicio, int fim) {
    if (inicio < fim) {
        int meio = inicio + (fim - inicio) / 2;

        mergeSortUsuario(arq, inicio, meio);
        mergeSortUsuario(arq, meio + 1, fim);

        mergeUsuario(arq, inicio, meio, fim);
    }
    return xcont;
}

//Seleção Natural para partições maiores
int selecaoNaturalUsuario(FILE *arqEntrada, int M, int *numComparacoes) {
    TUsuario *reservatorio = (TUsuario *)malloc(M * sizeof(TUsuario)); //reservatório do tamanho da entrada
    int posicaoReservatorio = 0;
    int numParticao = 1;

    //verifica se há alguma partição no diretório, se tiver, ele remove
    for (int i = 1; i <= MAX_PARTITIONS; i++) {
        char nomeParticaoExistente[20];
        sprintf(nomeParticaoExistente, "partitions_usuario/particao_%d.dat", i);
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
        sprintf(nomeParticao, "partitions_usuario/particao_%d.dat", numParticao);
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

/*
//Usuario cria playlist
void criarPlaylist(TUsuario *usuario, TPlaylist *playlist) {
    if(usuario->qtdPlaylists < MAX_PLAYLIST) {
        usuario->playlists[usuario->qtdPlaylists] = playlist;
        usuario->qtdPlaylists++;
    }
    else {
        printf("ERRO! Limite de playlists atingido!");
    }
}

//Usuario cria reviews
void criarReviews(TMusicas* music, TReviews *review) {
    music->review = review;
}
*/