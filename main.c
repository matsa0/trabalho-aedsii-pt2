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

int main() {
    //char imprimir;
    FILE *log;
    FILE *arqPlaylist;

    if ((log = fopen("log.txt", "w")) == NULL) {
        printf("ERRO! Não foi possível abrir o arquivo LOG!\n");
        exit(1);
    } else {
        if ((arqPlaylist = fopen("playlist.dat", "w+b")) == NULL) {
            printf("ERRO! Não foi possível encontrar o arquivo PLAYLIST!\n");
            exit(1);
        }
        else {
            criarBaseDesordenadaPlaylist(arqPlaylist, 50000);
            fclose(arqPlaylist);
        }

        if ((arqPlaylist = fopen("playlist.dat", "r+b")) == NULL) {
            printf("ERRO! Não foi possível encontrar o arquivo PLAYLIST!\n");
            exit(1);
        }
        else {
            selecaoNaturalPlaylist(arqPlaylist, 256);
            fclose(arqPlaylist);
        }

        fclose(log);
    }

    return 0;
}


