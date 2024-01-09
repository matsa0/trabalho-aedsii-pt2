#define _CRT_SECURE_NO_WARNINGS
#include "playlist.h"
#define MAX_MUSICAS 100
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "playlist.h"
#include "shuffle.h"
#include "limits.h"
#include <string.h>
#include <math.h>
#include "particoes.h"

//Retorna o tamanho da playlist em bytes
int tamanhoRegistroPlaylist() {
    return sizeof(int)  //id
           + sizeof(char) * 100 //nome
           + sizeof(char) * 200; //descrição
}

int tamanhoArquivoPlaylist(FILE * arq) {
    fseek(arq, 0, SEEK_END); //DESLOCA AT� O FINAL
    int tam = trunc(ftell(arq) / tamanhoRegistroPlaylist()); // ftel mede o deslocamento em bytes
    return tam;
}

//cria playlist
TPlaylist *playlist(int id, char *nome, char *descricao) {
    TPlaylist *playlist = (TPlaylist *) malloc(sizeof(TPlaylist));
    //inicializa espaco de memoria com ZEROS
    if (playlist) memset(playlist, 0, sizeof(TPlaylist));
    //copia valores para os campos de func
    playlist->id = id;
    strcpy(playlist->nome, nome); //usado para copiar as strings
    strcpy(playlist->descricao, descricao);
    return playlist;
}

// Retorna um ponteiro para a playlist lida do arquivo
TPlaylist *lePlaylist(FILE* in) {
    TPlaylist *playlist = (TPlaylist *) malloc(sizeof(TPlaylist));
    //O valor retornado por fread é comparado com zero para verificar se a leitura foi bem-sucedida. Se não for, a função libera a memória alocada para a playlist
    if (0 >= fread(&playlist->id, sizeof(int), 1, in)) {
        free(playlist);
        return NULL;
    }
    fread(playlist->nome, sizeof(char), sizeof(playlist->nome), in);
    fread(playlist->descricao, sizeof(char), sizeof(playlist->descricao), in);
    return playlist;
}

//salva playlist no arquivo out
void salvaPlaylist(TPlaylist *playlist, FILE *out) {
    fwrite(&playlist->id, sizeof(int), 1, out); //dado que vai escrever no arquivo, tamanho em bytes do tipo do dado, número de elementos a ser escrito, arquivo
    fwrite(playlist->nome, sizeof(char), sizeof(playlist->nome), out);
    fwrite(playlist->descricao, sizeof(char), sizeof(playlist->descricao), out);
}

// Cria a base de dados desordenada
void criarBaseDesordenadaPlaylist(FILE *out, int tam) {
    int vet[tam];
    TPlaylist *play;

    for(int i=0;i<tam;i++)
        vet[i] = i+1;

    shuffle(vet, tam, 0);

    printf("\n\nGerando a base de dados DESORDENADA de PLAYLIST...\n");

    for (int i=0;i<tam;i++){
        play = playlist(vet[i], "Hits internacionais", "As melhores e mais ouvidas musicas internacionais do momento!");
        salvaPlaylist(play, out);
        free(play);
    }

}

// Cria a base de dados ordenada
void criarBaseOrdenadaPlaylist(FILE *out, int tam) {
    int vet[tam];
    TPlaylist *play;

    for(int i=0;i<tam;i++)
        vet[i] = i+1;

    printf("\n\nGerando a base de dados ORDENADA de PLAYLIST...\n");

    for (int i=0;i<tam;i++){
        play = playlist(vet[i], "Hits Brasil", "As mais ouvidas musicas brasileiras de novembro!");
        salvaPlaylist(play, out);
        free(play);
    }

}

// Imprime playlist
void imprimePlaylist(TPlaylist *playlist) {
    printf("**********************************************");
    printf("\nID Playlist > %d", playlist->id);
    printf("\nNome > %s", playlist->nome);
    printf("\nDescricao > %s", playlist->descricao);
    printf("\n**********************************************");
}

// Imprime a base de dados
void imprimirBasePlaylist(FILE *out) {
    printf("\nImprimindo a base de dados PLAYLIST...\n");

    rewind(out);
    TPlaylist *playlist;

    while ((playlist = lePlaylist(out)) != NULL)
        imprimePlaylist(playlist);

    free(playlist);
}

int cont = 0;
//Mesclar duas partições ordenadas
void mergePlaylist(FILE *arq, int inicio, int meio, int fim) {
    int i, j, k;
    int n1 = meio - inicio + 1;
    int n2 = fim - meio;

    // Partições temporárias
    TPlaylist *esq = (TPlaylist *)malloc(n1 * sizeof(TPlaylist));
    TPlaylist *dir = (TPlaylist *)malloc(n2 * sizeof(TPlaylist));

    // Copia os dados para as partições temporárias
    fseek(arq, inicio * tamanhoRegistroPlaylist(), SEEK_SET);
    for (i = 0; i < n1; i++) {
        esq[i] = *lePlaylist(arq);
    }

    fseek(arq, (meio + 1) * tamanhoRegistroPlaylist(), SEEK_SET);
    for (j = 0; j < n2; j++) {
        dir[j] = *lePlaylist(arq);
    }

    // Mescla as partições temporárias em ordem no arquivo
    i = 0;
    j = 0;
    k = inicio;

    while (i < n1 && j < n2) {
        if (esq[i].id <= dir[j].id) {
            salvaPlaylist(&esq[i], arq);
            i++;
        } else {
            salvaPlaylist(&dir[j], arq);
            j++;
        }
        k++;
        cont++;
    }

    // Copia os elementos restantes, se houver, de esq[] para o arquivo
    while (i < n1) {
        salvaPlaylist(&esq[i], arq);
        i++;
        k++;
    }

    // Copia os elementos restantes, se houver, de dir[] para o arquivo
    while (j < n2) {
        salvaPlaylist(&dir[j], arq);
        j++;
        k++;
    }

    free(esq);
    free(dir);
}

//Ordenação por MergeSort
int mergeSortPlaylist(FILE *arq, int inicio, int fim) {
    if (inicio < fim) {
        int meio = inicio + (fim - inicio) / 2;

        mergeSortPlaylist(arq, inicio, meio);
        mergeSortPlaylist(arq, meio + 1, fim);

        mergePlaylist(arq, inicio, meio, fim);
    }
    return cont;
}

int selecaoNaturalPlaylist(FILE *arqEntrada, int M, int *numComparacoes) {
    TPlaylist *reservatorio = (TPlaylist *)malloc(M * sizeof(TPlaylist)); //reservatório do tamanho da entrada
    int posicaoReservatorio = 0;
    int numParticao = 1;

    //(Passo 1) Leitura inicial dos primeiros M registros do arquivo para o reservatório
    for (int i = 0; i < M; i++) {
        if (fread(&reservatorio[i], sizeof(TPlaylist), 1, arqEntrada) != 1) {
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
        sprintf(nomeParticao, "partitions_playlist/particao_%d.dat", numParticao);
        FILE *out = fopen(nomeParticao, "w+b");
        fwrite(&reservatorio[menorChave], sizeof(TPlaylist), 1, out);
        fclose(out);

        //(Passo 4) Substituir, no reservatório, o registro r pelo próximo registro do arqEntrada
        if (fread(&reservatorio[menorChave], sizeof(TPlaylist), 1, arqEntrada) != 1) {
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
            if (fread(&reservatorio[i], sizeof(TPlaylist), 1, arqEntrada) != 1) {
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
//escrever a busca sequencial no log
void logBuscaSequencialPlaylist(FILE* out, int count, clock_t start_time) {
    clock_t end_time = clock();
    fprintf(out, "\n------------------------------");
    fprintf(out, "\nBusca sequencial PLAYLIST");
    fprintf(out, "\nNumero de comparações > %d", count);
    fprintf(out, "\nTempo de Busca > %.2f segundos", (double)(end_time - start_time) / CLOCKS_PER_SEC);
    fprintf(out, "\n------------------------------");
    fprintf(out, "\n");
}

//busca sequencial sobre a base
TPlaylist *buscaSequencialPlaylist(int chave, FILE *in, FILE* out) {
    TPlaylist *playlist;
    int achou = 0;
    rewind(in);
    int count = 0;
    clock_t start_time = clock();

    while ((playlist = lePlaylist(in)) != NULL){
        if(playlist->id == chave){
            //return f;
            achou = 1;
            break;
        }
        count++;
    }

    if(achou == 1) {
        logBuscaSequencialPlaylist(out, count, start_time);
        return playlist;
    }
    else {
        printf("ERRO! PLAYLIST nao encontrada.");
        logBuscaSequencialPlaylist(out, count, start_time);
    }


    free(playlist);
    return NULL;
}

//escrever a busca binária no log
void logBuscaBinariaPlaylist(FILE* out, int count, clock_t start_time) {
    clock_t end_time = clock();
    fprintf(out, "\n------------------------------");
    fprintf(out, "\nBusca binária PLAYLIST");
    fprintf(out, "\nNumero de comparações > %d", count);
    fprintf(out, "\nTempo de Busca > %.2f segundos", (double)(end_time - start_time) / CLOCKS_PER_SEC);
    fprintf(out, "\n------------------------------");
    fprintf(out, "\n");
}

//busca binária sobre a base
TPlaylist *buscaBinariaPlaylist(int chave, FILE *in, FILE *out, int inicio, int fim) {
    TPlaylist *playlist = NULL;
    int cod = -1;
    int count = 0;
    clock_t start_time;
    start_time = clock();

    while (inicio <= fim && cod != chave) {
        int meio = trunc((inicio + fim) / 2);
        //printf("\nInicio: %d; Fim: %d; Meio: %d\n", inicio, fim, meio);
        fseek(in, (meio -1) * tamanhoRegistroPlaylist(), SEEK_SET);
        playlist = lePlaylist(in);
        cod = &playlist->id;

        if (playlist) {
            if (cod > chave) {
                fim = meio - 1;
            } else {
                inicio = meio + 1;
            }
        }

        count++;
    }

    if (cod == chave) {
        logBuscaBinariaPlaylist(out, count, start_time);
        return playlist;
    }

    logBuscaBinariaPlaylist(out, count, start_time);

    return NULL;
}


void insertionSortPlaylist(FILE *arq, FILE *log, int tam) {
    clock_t start, end;
    double tempoTotal;

    start = clock();
    int i;
    //faz o insertion sort
    for (int j = 2; j <= tam; j++) {
        //posiciona o arquivo no registro j
        fseek(arq, (j-1) * tamanhoRegistroPlaylist(), SEEK_SET);
        TPlaylist *fj = lePlaylist(arq);
        i = j - 1;
        //posiciona o cursor no registro i
        fseek(arq, (i-1) * tamanhoRegistroPlaylist(), SEEK_SET);
        TPlaylist *fi = lePlaylist(arq);
        while ((i > 0) && (fi->id > fj->id)) {
            //posiciona o cursor no registro i+1
            fseek(arq, i * tamanhoRegistroPlaylist(), SEEK_SET);
            salvaPlaylist(fi, arq);
            i = i - 1;
            //lê registro i
            fseek(arq, (i-1) * tamanhoRegistroPlaylist(), SEEK_SET);
            fi = lePlaylist(arq);
        }
        //posiciona cursor no registro i + 1
        fseek(arq, (i) * tamanhoRegistroPlaylist(), SEEK_SET);
        //salva registro j na posição i
        salvaPlaylist(fj, arq);
    }
    end = clock();

    tempoTotal = ((double)(end - start)) / CLOCKS_PER_SEC;

    fprintf(log, "Tempo gasto para ordenar com o Insertion Sort > %.2f", tempoTotal);
    fprintf(log, "\n------------------------------\n");

    //descarrega o buffer para ter certeza que dados foram gravados
    fflush(arq);
}


//Classificação interna gera partições classificadas de M registros!
int classificacaoInternaPlaylist(FILE *arq, int M) {

    rewind(arq); //posiciona cursor no inicio do arquivo

    int reg = 0;
    int nPlay = tamanhoArquivoPlaylist(arq);
    int qtdParticoes = 0;
    int t = tamanhoRegistroPlaylist();
    char *nomeParticao[20];

    //O loop continuará até que todos os registros do arquivo tenham sido processados
    while (reg != nPlay) {
        //le o arquivo e coloca no vetor
        TPlaylist *v[M];
        int i = 0;
        while (!feof(arq)) {
            fseek(arq, (reg) * t, SEEK_SET);
            v[i] = lePlaylist(arq);
            //     imprime_funcionario(v[i]);
            i++;
            reg++;
            if(i>=M) break;
        }

        //ajusta tamanho M caso arquivo de entrada tenha terminado antes do vetor
        if (i != M) {
            M = i;
        }

        //faz ordenacao com insertion sort
        for (int j = 1; j < M; j++) {
            TPlaylist *f = v[j];
            i = j - 1;
            while ((i >= 0) && (v[i]->id >  f->id)) {
                v[i + 1] = v[i];
                i = i - 1;
            }
            v[i + 1] = f;
        }

        //cria arquivo de particao e faz gravacao

        sprintf(nomeParticao, "partitions_playlist/partition%i.dat", qtdParticoes);
        //nome = fopen(nomeParticao, "wb");

        //printf("\n%s\n", nome);

        FILE *p;

        if ((p = fopen(nomeParticao, "wb+")) == NULL) { //escrita binária
            printf("Erro criar arquivo de saida\n");
        } else {
            for (int i = 0; i < M; i++) {
                fseek(p, (i) * t, SEEK_SET);
                salvaPlaylist(v[i], p);
                //imprime(v[i]);
            }
            //imprimirBase(p);
            fclose(p);
            qtdParticoes++;
        }
        for(int jj = 0; jj<M; jj++)
            free(v[jj]);
    }

    return qtdParticoes;
}

//Junta as partições classificadas , gerando um arquivo classificado
void intercalacaoBasicaPlaylist(FILE *out, int num_p) {
    typedef struct vetor{
        TPlaylist *play;
        FILE *f;
    }TVet;

    int fim = 0; //variavel que controla fim do procedimento
    int particao = 0;
    char *nome[20];


    //cria vetor de particoes
    TVet v[num_p];

    //abre arquivos das particoes, colocando variavel de arquivo no campo f do vetor
    //e primeiro funcionario do arquivo no campo func do vetor
    for (int i=0; i < num_p; i++) {

        sprintf(nome, "partitions_playlist/partition%i.dat", particao);

        //printf("%s",nome);

        v[i].f = fopen(nome, "rb");
        //v[i].aux_p = 0;

        if (v[i].f != NULL) {
            //fseek(v[i].f, v[i].aux_p * tamanho(), SEEK_SET);
            TPlaylist *p = lePlaylist(v[i].f);
            if (p == NULL) {
                //arquivo estava vazio
                //coloca HIGH VALUE nessa posi??o do vetor
                v[i].play = playlist(INT_MAX, "","");
            }
            else {
                //conseguiu ler funcionario, coloca na posi??o atual do vetor
                v[i].play = p;
            }
        }
        else {
            fim = 1;
        }

        particao++;
    }

    //int aux = 0;

    while (!(fim)) { //conseguiu abrir todos os arquivos
        int menor = INT_MAX;
        int pos_menor;
        //encontra o funcionario com menor chave no vetor
        for(int i = 0; i < num_p; i++){
            if(v[i].play->id < menor){
                menor = v[i].play->id;
                pos_menor = i;
            }
        }
        if (menor == INT_MAX) {
            fim = 1; //terminou processamento
        }
        else {
            //salva funcionario no arquivo de saída
            //fseek(out, aux * tamanho(), SEEK_SET);
            salvaPlaylist(v[pos_menor].play, out);
            //printf("%d ",pos_menor);
            //atualiza posição pos_menor do vetor com pr?ximo funcionario do arquivo
            //v[pos_menor].aux_p++;
            //fseek(v[pos_menor].f, v[pos_menor].aux_p * tamanho(), SEEK_SET);
            TPlaylist *p = lePlaylist(v[pos_menor].f);
            //aux++;
            if (p == NULL) {
                //arquivo estava vazio
                //coloca HIGH VALUE nessa posiçao do vetor
                v[pos_menor].play = playlist(INT_MAX, "", "");
            }
            else {
                v[pos_menor].play = p;
            }

        }
    }

    //fecha arquivos das partiÇões de entrada
    for(int i = 0; i < num_p; i++){
        fclose(v[i].f);
        //    free(v[i].func);
    }
    //fecha arquivo de saída
    //fclose(out);


}


//Adiciona musica a playlist
void adicionarMusica(TPlaylist* playlist, TMusicas* musica) {
    if(playlist->qntMusicas < MAX_MUSICAS) {
        playlist->musicas[playlist->qntMusicas] = musica;
        playlist->qntMusicas++;
    }
    else {
        printf("\nERRO! Limite de musicas da playlist atingido!");
    }
}
 */