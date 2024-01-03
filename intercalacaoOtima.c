#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "intercalacaoOtima.h"

#define F 10 // Número de partições a serem intercaladas em cada fase

void intercalacaoOtima(const char **nomeParticoes, const char *nomeArquivoSaida, int numParticoes) {
    FILE **particoes = (FILE **)malloc(F * sizeof(FILE *));
    FILE *arquivoSaida = fopen(nomeArquivoSaida, "w");

    int indicesParticoes[F];
    int i, j, menorIndice;

    while(numParticoes > 0) {
        for(i = 0;  i < F && numParticoes > 0; i++) {
            particoes[i] = fopen(nomeParticoes[i], "r");
            indicesParticoes[i] = 0;
            numParticoes--;
        }

        while(1) {
            menorIndice = -1;

            for(i = 0; i < F; i++) {
                if(particoes[i] != NULL) {
                    if(menorIndice == -1 || particoes[i] < particoes[menorIndice]) {
                        menorIndice = i;
                    }
                }
            }

            if(menorIndice == -1) {
                break;
            }

            fprintf(arquivoSaida, "%d ", indicesParticoes[menorIndice]++);

            if(fscanf(particoes[menorIndice], "%d", &j) != 1) {
                fclose(particoes[menorIndice]);
                particoes[menorIndice] = NULL;
            }
        }
    }

    fclose(arquivoSaida);
    free(particoes);
}