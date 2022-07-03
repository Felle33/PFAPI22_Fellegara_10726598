#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define HASHMAPSIZE 13

typedef struct nodeHash{
    char c;
    int num;
    struct nodeHash* pn;
}nodeHash;

typedef struct nodeStringa{
    char* s;
    bool valid;
    struct nodeStringa* pn;
    nodeHash** ph;
}nodeStringa;

void setLengthBuff(int *lengthBuff, int k){
    if(k > 17)
        *lengthBuff = k + 1;
    else
        *lengthBuff = 18;
}

void initializeHashMap(nodeHash*** hashMap){
    for(int i = 0; i < HASHMAPSIZE; i++)
        (*hashMap)[i] = NULL;
}

// Cerca se esiste un carattere nell'hashMap e ritorna il suo puntatore se esiste
nodeHash* searchHashMapCar(nodeHash*** hashMap, char car){
    int cella = car % HASHMAPSIZE;
    nodeHash* headList = (*hashMap)[cella];

    while(headList != NULL){
        if(headList->c == car)
            return headList;

        headList = headList->pn;
    }
    return NULL;
}

// Inserisce una lettera nell'hashMap
void insertHashMap(nodeHash*** hashMap, char car, int pos){
    nodeHash* headList = searchHashMapCar(hashMap, car);

    if(headList != NULL){
        headList->num++;
        return;
    }
    else{
        nodeHash* ins = (nodeHash*) malloc(sizeof(nodeHash));
        ins->c = car;
        ins->pn = NULL;
        ins->num = 1;

        int cella = car % HASHMAPSIZE;
        ins->pn = (*hashMap)[cella];
        (*hashMap)[cella] = ins;
    }
}

void createHashMap(nodeStringa* ins){
    ins->ph = (nodeHash**) malloc(sizeof(nodeHash*) * HASHMAPSIZE);
    initializeHashMap(&ins->ph);
    char* c = ins->s;
    int pos = 0;

    while(*c != '\0'){
        insertHashMap(&(ins->ph), *c, pos);
        pos++;
        c++;
    }
}

void insertLista(nodeStringa** list, char* buffer, int k){
    nodeStringa* curr = *list, *prec = NULL;

    while(curr != NULL && strcmp(curr->s, buffer) < 0){
        prec = curr;
        curr = curr->pn;
    }

    nodeStringa* ins = (nodeStringa*) malloc(sizeof(nodeStringa));
    ins->pn = curr;
    ins->valid = true;
    ins->s = (char*) malloc(sizeof(char) * k);
    strcpy(ins->s, buffer);
    ins->ph = NULL;
    createHashMap(ins);

    if(prec != NULL)
        prec->pn = ins;
    else
        *list = ins;

}

void creazioneParole(nodeStringa** list, int lengthBuff, int k){
    char* buffer = malloc(sizeof(char) * lengthBuff);

    do{
        fgets(buffer, lengthBuff, stdin);
        insertLista(list, buffer, k);

    } while (strcmp(buffer, "+inizio_partita") == 0);
}

void freeParole(nodeStringa** list){

}

int main(){
    int k, lengthBuff;
    nodeStringa* list = NULL;

    if(scanf("%d\n", &k) == EOF) exit(-1);
    setLengthBuff(&lengthBuff, k);

    creazioneParole(&list, lengthBuff, k);

    freeParole(&list);
    return 0;
}
