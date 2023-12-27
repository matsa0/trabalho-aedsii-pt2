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
    sleep(1);
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


//Classificação interna Usuarios
int classificacaoInternaUsuario(FILE *arq, int M) {
    rewind(arq); //posiciona cursor no inicio do arquivo

    int reg = 0;
    int nUser = tamanhoArquivoUsuario(arq);
    int qtdParticoes = 0;
    int t = tamanhoRegistroUsuario();
    char *nomeParticao[20];

    //O loop continuará até que todos os registros do arquivo tenham sido processados
    while (reg != nUser) {
        //le o arquivo e coloca no vetor
        TUsuario *v[M];
        int i = 0;
        while (!feof(arq)) {
            fseek(arq, (reg) * t, SEEK_SET);
            v[i] = leUsuario(arq);
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
            TUsuario *f = v[j];
            i = j - 1;
            while ((i >= 0) && (v[i]->id >  f->id)) {
                v[i + 1] = v[i];
                i = i - 1;
            }
            v[i + 1] = f;
        }

        //cria arquivo de particao e faz gravacao

        sprintf(nomeParticao, "partitions_usuario/partition%i.dat", qtdParticoes);
        //nome = fopen(nomeParticao, "wb");

        //printf("\n%s\n", nome);

        FILE *p;

        if ((p = fopen(nomeParticao, "wb+")) == NULL) { //escrita binária
            printf("Erro criar arquivo de saida\n");
        } else {
            for (int i = 0; i < M; i++) {
                fseek(p, (i) * t, SEEK_SET);
                salvaUsuario(v[i], p);
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
void intercalacaoBasicaUsuario(FILE *out, int num_p) {
    typedef struct vetor{
        TUsuario *usr;
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

        sprintf(nome, "partitions_usuario/partition%i.dat", particao);

        //printf("%s",nome);

        v[i].f = fopen(nome, "rb");
        //v[i].aux_p = 0;

        if (v[i].f != NULL) {
            //fseek(v[i].f, v[i].aux_p * tamanho(), SEEK_SET);
            TUsuario *p = leUsuario(v[i].f);
            if (p == NULL) {
                //arquivo estava vazio
                //coloca HIGH VALUE nessa posi??o do vetor
                v[i].usr = usuario(INT_MAX, "", "");
            }
            else {
                //conseguiu ler funcionario, coloca na posi??o atual do vetor
                v[i].usr = p;
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
            if(v[i].usr->id < menor){
                menor = v[i].usr->id;
                pos_menor = i;
            }
        }
        if (menor == INT_MAX) {
            fim = 1; //terminou processamento
        }
        else {
            //salva funcionario no arquivo de saída
            //fseek(out, aux * tamanho(), SEEK_SET);
            salvaUsuario(v[pos_menor].usr, out);
            //printf("%d ",pos_menor);
            //atualiza posição pos_menor do vetor com pr?ximo funcionario do arquivo
            //v[pos_menor].aux_p++;
            //fseek(v[pos_menor].f, v[pos_menor].aux_p * tamanho(), SEEK_SET);
            TUsuario *p = leUsuario(v[pos_menor].f);
            //aux++;
            if (p == NULL) {
                //arquivo estava vazio
                //coloca HIGH VALUE nessa posiçao do vetor
                v[pos_menor].usr = usuario(INT_MAX, "", "");
            }
            else {
                v[pos_menor].usr = p;
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
