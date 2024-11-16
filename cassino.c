#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUMERO_DE_JOGADORES 10
#define PORCENTAGEM_DE_GANHAR 25
#define PORCENTAGEM_DE_APOSTA 25
#define PORCENTAGEM_RETORNADA 100
#define DINHEIRO_INICIAL_DO_CASSINO 1000
#define DINHEIRO_INICIAL_DOS_JOGADORES 100
#define NUMERO_DE_RODADAS 4

float dinheiro_cassino = DINHEIRO_INICIAL_DO_CASSINO;

typedef struct{
    int id;
    float dinheiro;
}Jogador;

void* jogar(void* args){
    Jogador* jogador = (Jogador*)args;
    for(int i = 0; i < NUMERO_DE_RODADAS; i++){
        if(dinheiro_cassino <= 0 || jogador->dinheiro <= 0){
            break;
        }

        float dinheiro_apostado = jogador->dinheiro * PORCENTAGEM_DE_APOSTA / 100;
        unsigned int sorte = rand() % 100;
        if(sorte < PORCENTAGEM_DE_GANHAR){
            jogador->dinheiro = jogador->dinheiro + dinheiro_apostado * PORCENTAGEM_RETORNADA / 100;
        }
        else{
            jogador->dinheiro -= dinheiro_apostado;
            dinheiro_cassino += dinheiro_apostado;
        }
    }

    pthread_exit(NULL);
}

int main(){
    srand(time(NULL));
    Jogador* jogadores = (Jogador*) malloc(NUMERO_DE_JOGADORES * sizeof(Jogador));
    pthread_t threads[NUMERO_DE_JOGADORES]; 

    for(int i = 0; i < NUMERO_DE_JOGADORES; i++){
        jogadores[i].id = i;
        jogadores[i].dinheiro = DINHEIRO_INICIAL_DOS_JOGADORES;
        pthread_create(&threads[i], NULL, jogar, (void*)&jogadores[i]);
    }

    for (int i = 0; i < NUMERO_DE_JOGADORES; i++) {
        printf("Jogador %d: %.2f reais\n", jogadores[i].id, jogadores[i].dinheiro);
        pthread_join(threads[i], NULL);
    }
    printf("Cassino: %.2f reais\n", dinheiro_cassino);

    free(jogadores);
    return 0;
}