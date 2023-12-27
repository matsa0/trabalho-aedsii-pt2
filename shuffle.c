//
// Created by MATHEUS on 27/12/2023.
//

#include "shuffle.h"
#include <stdio.h>
#include <stdlib.h>

void shuffle(int *vet,int MAX,int MIN) {
    srand(time(NULL));
    for (int i = MAX - MIN - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = vet[j];
        vet[j] = vet[i];
        vet[i] = tmp;
    }
}