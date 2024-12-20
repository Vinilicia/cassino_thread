#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUMERO_DE_JOGADORES 10
#define PORCENTAGEM_DE_GANHAR 40
#define PORCENTAGEM_DE_APOSTA 25
#define PORCENTAGEM_RETORNADA 100
#define DINHEIRO_INICIAL_DO_CASSINO 1000
#define DINHEIRO_INICIAL_DOS_JOGADORES 100
#define NUMERO_DE_RODADAS 1000

double dinheiro_cassino = DINHEIRO_INICIAL_DO_CASSINO;
int usar_mutex = 0;

pthread_mutex_t mutex_cassino;

typedef struct{
    int id;
    double dinheiro;
}Jogador;

void* jogar(void* args){
    Jogador* jogador = (Jogador*)args;

    for(int i = 0; i < NUMERO_DE_RODADAS; i++){
        if(jogador->dinheiro <= 0){
            printf("Jogador %d quebrou\n", jogador->id + 1);
            break;
        }

        double dinheiro_apostado = jogador->dinheiro * PORCENTAGEM_DE_APOSTA / 100;
        unsigned int sorte = rand() % 100;
        if(sorte < PORCENTAGEM_DE_GANHAR){
            jogador->dinheiro = jogador->dinheiro + dinheiro_apostado * PORCENTAGEM_RETORNADA / 100;
            if(usar_mutex){
                pthread_mutex_lock(&mutex_cassino);
            }
            dinheiro_cassino = dinheiro_cassino - dinheiro_apostado * PORCENTAGEM_RETORNADA / 100;
            if(usar_mutex){
                pthread_mutex_unlock(&mutex_cassino);
            }
        }
        else{
            jogador->dinheiro -= dinheiro_apostado;
            if(usar_mutex){
                pthread_mutex_lock(&mutex_cassino);
            }
            dinheiro_cassino += dinheiro_apostado;
            if(usar_mutex){
                pthread_mutex_unlock(&mutex_cassino);
            }
        }
    }

    pthread_exit(NULL);
}

int main(){
    srand(time(NULL));

    Jogador* jogadores = (Jogador*) malloc(NUMERO_DE_JOGADORES * sizeof(Jogador));
    pthread_t threads[NUMERO_DE_JOGADORES];
    pthread_mutex_init(&mutex_cassino, NULL);

    for(int i = 0; i < NUMERO_DE_JOGADORES; i++){
        jogadores[i].id = i;
        jogadores[i].dinheiro = DINHEIRO_INICIAL_DOS_JOGADORES;
        pthread_create(&threads[i], NULL, jogar, (void*)&jogadores[i]);
    }

    for (int i = 0; i < NUMERO_DE_JOGADORES; i++) {
        pthread_join(threads[i], NULL);
    }

    double dinheiro_antes = DINHEIRO_INICIAL_DO_CASSINO + NUMERO_DE_JOGADORES * DINHEIRO_INICIAL_DOS_JOGADORES;
    double dinheiro_depois = dinheiro_cassino;

    for (int i = 0; i < NUMERO_DE_JOGADORES; i++) {
        dinheiro_depois += jogadores[i].dinheiro;
        printf("Jogador %d: %.2f reais\n", jogadores[i].id+1, jogadores[i].dinheiro);
    }
    printf("Cassino: %.2f reais\n\n", dinheiro_cassino);

    printf("Dinheiro antes = %.2f reais\nDinhero depois = %.2f reais\n", dinheiro_antes, dinheiro_depois);

    free(jogadores); 
    pthread_mutex_destroy(&mutex_cassino);
    return 0;
}