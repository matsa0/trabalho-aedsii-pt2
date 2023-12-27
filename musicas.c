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
    sleep(1);
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
    sleep(1);
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

//Classificação interna Musicas
int classificacaoInternaMusicas(FILE *arq, int M) {
    rewind(arq); //posiciona cursor no inicio do arquivo

    int reg = 0;
    int nMsc = tamanhoArquivoMusicas(arq);
    int qtdParticoes = 0;
    int t = tamanhoRegistroMusicas();
    char *nomeParticao[20];

    //O loop continuará até que todos os registros do arquivo tenham sido processados
    while (reg != nMsc) {
        //le o arquivo e coloca no vetor
        TMusicas *v[M];
        int i = 0;
        while (!feof(arq)) {
            fseek(arq, (reg) * t, SEEK_SET);
            v[i] = leMusicas(arq);
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
            TMusicas *f = v[j];
            i = j - 1;
            while ((i >= 0) && (v[i]->id >  f->id)) {
                v[i + 1] = v[i];
                i = i - 1;
            }
            v[i + 1] = f;
        }

        //cria arquivo de particao e faz gravacao

        sprintf(nomeParticao, "partitions_musicas/partition%i.dat", qtdParticoes);
        //nome = fopen(nomeParticao, "wb");

        //printf("\n%s\n", nome);

        FILE *p;

        if ((p = fopen(nomeParticao, "wb+")) == NULL) { //escrita binária
            printf("Erro criar arquivo de saida\n");
        } else {
            for (int i = 0; i < M; i++) {
                fseek(p, (i) * t, SEEK_SET);
                salvaMusicas(v[i], p);
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
void intercalacaoBasicaMusicas(FILE *out, int num_p) {
    typedef struct vetor{
        TMusicas *msc;
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

        sprintf(nome, "partitions_musicas/partition%i.dat", particao);

        //printf("%s",nome);

        v[i].f = fopen(nome, "rb");
        //v[i].aux_p = 0;

        if (v[i].f != NULL) {
            //fseek(v[i].f, v[i].aux_p * tamanho(), SEEK_SET);
            TMusicas *p = leMusicas(v[i].f);
            if (p == NULL) {
                //arquivo estava vazio
                //coloca HIGH VALUE nessa posi??o do vetor
                v[i].msc = musicas(INT_MAX, "","","");
            }
            else {
                //conseguiu ler funcionario, coloca na posi??o atual do vetor
                v[i].msc = p;
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
            if(v[i].msc->id < menor){
                menor = v[i].msc->id;
                pos_menor = i;
            }
        }
        if (menor == INT_MAX) {
            fim = 1; //terminou processamento
        }
        else {
            //salva funcionario no arquivo de saída
            //fseek(out, aux * tamanho(), SEEK_SET);
            salvaMusicas(v[pos_menor].msc, out);
            //printf("%d ",pos_menor);
            //atualiza posição pos_menor do vetor com pr?ximo funcionario do arquivo
            //v[pos_menor].aux_p++;
            //fseek(v[pos_menor].f, v[pos_menor].aux_p * tamanho(), SEEK_SET);
            TMusicas *p = leMusicas(v[pos_menor].f);
            //aux++;
            if (p == NULL) {
                //arquivo estava vazio
                //coloca HIGH VALUE nessa posiçao do vetor
                v[pos_menor].msc = musicas(INT_MAX,"", "", "");
            }
            else {
                v[pos_menor].msc = p;
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

//Adiciona review para a música
void adicionaReview(TMusicas* musica, TReviews* review) {
    musica->review = review;
}
