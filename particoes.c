#include "particoes.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>


//lista os arquivos presentes em um diretório
void capturaParticoes(const char *caminhoPasta, char **particoes, int *numParticoes) {
    DIR * dir;
    struct dirent *entrada; //contém informações sobre o arquivo encontrado no diretório
    int count = 0;

    if((dir = opendir(caminhoPasta)) != NULL) { //abrindo a pasta
        //verifica cada entrada do diretório
        while((entrada = readdir(dir)) != NULL) {
            char caminhoCompleto[FILENAME_MAX]; //o tamanho máximo do caminho de arquivo permitido no sistema
            snprintf(caminhoCompleto, FILENAME_MAX, "%s/%s", caminhoPasta, entrada->d_name);

            struct stat fileStat;
            if (stat(caminhoCompleto, &fileStat) == 0) { //obter informações sobre o arquivo passndo o caminho completo dele
                //verifica se o arquivo é regular e verifica se o arquivo tem .dat no nome
                if (S_ISREG(fileStat.st_mode) && strstr(entrada->d_name, ".dat") != NULL) {
                    particoes[count] = malloc(strlen(caminhoCompleto) + 1);
                    strcpy(particoes[count], caminhoCompleto);
                    count++;
                }
            }
        }
        closedir(dir);
    } else {
        perror("ERRO! Não foi possível abrir o diretório.");
        exit(EXIT_FAILURE);
    }
    *numParticoes = count;
}