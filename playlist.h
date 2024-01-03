//
// Created by MATHEUS on 27/12/2023.
//

#ifndef TP02_AEDSII_CSI104_PLAYLIST_H
#define TP02_AEDSII_CSI104_PLAYLIST_H
#define MAX_MUSICAS 100
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "musicas.h"

typedef struct Playlist {
    int id;
    char nome[100];
    char descricao[200];
    TMusicas *musicas[MAX_MUSICAS];
    int qntMusicas;
} TPlaylist;

//cria playlist
TPlaylist *playlist(int id, char *nome, char *descricao);

// Retorna um ponteiro para a playlist lida do arquivo
TPlaylist *lePlaylist(FILE* in);

//salva playlist
void salvaPlaylist(TPlaylist *playlist, FILE *out);

// Cria a base de dados desordenada
void criarBaseDesordenadaPlaylist(FILE *out, int tam);

// Cria a base de dados ordenada
void criarBaseOrdenadaPlaylist(FILE *out, int tam);

// Imprime playlist
void imprimePlaylist(TPlaylist *playlist);

// Imprime a base de dados
void imprimirBasePlaylist(FILE *out);

//escrever a busca sequencial no log
void logBuscaSequencialPlaylist(FILE* out, int count, clock_t start_time);

//busca sequencial sobre a base
TPlaylist *buscaSequencialPlaylist(int chave, FILE *in, FILE* out);

//escrever a busca binária no log
void logBuscaBinariaPlaylist(FILE* out, int count, clock_t start_time);

//busca binária sobre a base
TPlaylist *buscaBinariaPlaylist(int chave, FILE *in, FILE *out, int inicio, int fim);

//Retorna o tamnanho da playlist em bytes
int tamanhoRegistroPlaylist();

//Retorna o tamanho do arquivo PLaylist
int tamanhoArquivoPlaylist(FILE * arq);

//Mesclar duas partições ordenadas
void mergePlaylist(FILE *arq, int inicio, int meio, int fim);

//Ordenação por MergeSort
int mergeSortPlaylist(FILE *arq, int inicio, int fim);

//Seleção Natural para partições maiores
void selecaoNaturalPlaylist(FILE *arqEntrada, int M, int *numComparacoes);
/*
//ordena base de dados com insertion sort
void insertionSortPlaylist(FILE *arq, FILE *log, int tam);

//Adiciona musica a playlist
void adicionarMusica(TPlaylist* playlist, TMusicas* musicas);

//Classificação interna Playlist
int classificacaoInternaPlaylist(FILE *arq, int M);

//Junta as partições classificadas , gerando um arquivo classificado
void intercalacaoBasicaPlaylist(FILE *out, int num_p);
*/

#endif //TP02_AEDSII_CSI104_PLAYLIST_H
