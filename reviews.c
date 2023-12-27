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

//Classificação interna Reviews
int classificacaoInternaReviews(FILE *arq, int M) {
    rewind(arq); //posiciona cursor no inicio do arquivo

    int reg = 0;
    int nRev = tamanhoArquivoReviews(arq);
    int qtdParticoes = 0;
    int t = tamanhoRegistroReviews();
    char *nomeParticao[20];

    //O loop continuará até que todos os registros do arquivo tenham sido processados
    while (reg != nRev) {
        //le o arquivo e coloca no vetor
        TReviews *v[M];
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
            TReviews *f = v[j];
            i = j - 1;
            while ((i >= 0) && (v[i]->id >  f->id)) {
                v[i + 1] = v[i];
                i = i - 1;
            }
            v[i + 1] = f;
        }

        //cria arquivo de particao e faz gravacao

        sprintf(nomeParticao, "partitions_reviews/partition%i.dat", qtdParticoes);
        //nome = fopen(nomeParticao, "wb");

        //printf("\n%s\n", nome);

        FILE *p;

        if ((p = fopen(nomeParticao, "wb+")) == NULL) { //escrita binária
            printf("Erro criar arquivo de saida\n");
        } else {
            for (int i = 0; i < M; i++) {
                fseek(p, (i) * t, SEEK_SET);
                salvaReviews(v[i], p);
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
void intercalacaoBasicaReviews(FILE *out, int num_p) {
    typedef struct vetor{
        TReviews *rev;
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

        sprintf(nome, "partitions_reviews/partition%i.dat", particao);

        //printf("%s",nome);

        v[i].f = fopen(nome, "rb");
        //v[i].aux_p = 0;

        if (v[i].f != NULL) {
            //fseek(v[i].f, v[i].aux_p * tamanho(), SEEK_SET);
            TReviews *p = leReviews(v[i].f);
            if (p == NULL) {
                //arquivo estava vazio
                //coloca HIGH VALUE nessa posi??o do vetor
                v[i].rev = reviews(INT_MAX, "");
            }
            else {
                //conseguiu ler funcionario, coloca na posi??o atual do vetor
                v[i].rev = p;
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
            if(v[i].rev->id < menor){
                menor = v[i].rev->id;
                pos_menor = i;
            }
        }
        if (menor == INT_MAX) {
            fim = 1; //terminou processamento
        }
        else {
            //salva funcionario no arquivo de saída
            //fseek(out, aux * tamanho(), SEEK_SET);
            salvaReviews(v[pos_menor].rev, out);
            //printf("%d ",pos_menor);
            //atualiza posição pos_menor do vetor com pr?ximo funcionario do arquivo
            //v[pos_menor].aux_p++;
            //fseek(v[pos_menor].f, v[pos_menor].aux_p * tamanho(), SEEK_SET);
            TReviews *p = leReviews(v[pos_menor].f);
            //aux++;
            if (p == NULL) {
                //arquivo estava vazio
                //coloca HIGH VALUE nessa posiçao do vetor
                v[pos_menor].rev = reviews(INT_MAX, "");
            }
            else {
                v[pos_menor].rev = p;
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