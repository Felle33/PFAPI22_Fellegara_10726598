#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define HASHMAPSIZE 13

typedef struct nodeLetter{
    int pos;
    struct nodeLetter* pn;
    bool connected;
}nodeLetter;

typedef struct nodeHash{
    char c;
    int num;
    struct nodeHash* pn;
    nodeLetter* pl;
}nodeHash;

typedef struct nodeString{
    char* s;
    bool valid;
    struct nodeString* pn;
    nodeHash** ph;
}nodeString;

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
        nodeLetter* l = (nodeLetter*) malloc(sizeof(nodeLetter));

        headList->num++;
        l->connected = false;
        l->pos = pos;
        l->pn = headList->pl;
        headList->pl = l;
    }
    else{
        nodeHash* ins = (nodeHash*) malloc(sizeof(nodeHash));
        nodeLetter* l = (nodeLetter*) malloc(sizeof(nodeLetter));

        ins->c = car;
        ins->pn = NULL;
        ins->num = 1;
        ins->pl = l;

        l->pos = pos;
        l->pn = NULL;
        l->connected = true;

        int cella = car % HASHMAPSIZE;
        ins->pn = (*hashMap)[cella];
        (*hashMap)[cella] = ins;
    }
}

void createHashMap(nodeString* s){
    s->ph = (nodeHash**) malloc(sizeof(nodeHash*) * HASHMAPSIZE);
    initializeHashMap(&s->ph);
    char* c = s->s;
    int pos = 0;

    while(*c != '\0'){
        insertHashMap(&(s->ph), *c, pos);
        pos++;
        c++;
    }
}

void insertLista(nodeString** list, char* buffer, int k){
    nodeString* curr = *list, *prev = NULL;

    while(curr != NULL && strcmp(curr->s, buffer) < 0){
        prev = curr;
        curr = curr->pn;
    }

    nodeString* ins = (nodeString*) malloc(sizeof(nodeString));
    ins->pn = curr;
    ins->valid = true;
    ins->s = (char*) malloc(sizeof(char) * k);
    strncpy(ins->s, buffer, k);
    ins->ph = NULL;

    if(prev != NULL)
        prev->pn = ins;
    else
        *list = ins;
}

nodeString *searchList(nodeString* list, char *buffer) {
    while(list != NULL){
        if(strcmp(list->s, buffer) == 0)
            return list;
        list = list->pn;
    }
    return NULL;
}

void creazioneParole(nodeString** list, int lengthBuff, int k, char* endString){
    char* buffer = malloc(sizeof(char) * lengthBuff);

    while(1){
        if(scanf("%[^\n]s", buffer) == EOF) exit(-2);
        while ((getchar()) != '\n');
        if(strcmp(buffer, endString) == 0) break;
        insertLista(list, buffer, k);
    }

    free(buffer);
}

void freeLista(nodeString** list){
    nodeString *curr = *list, *suc = NULL;

    while(curr != NULL){
        suc = curr->pn;
        free(curr->s);
        free(curr);
        curr = suc;
    }
}

void freeHashMap(nodeHash*** hashMap){
    for(int i = 0; i < HASHMAPSIZE; i++){
        nodeHash *curH = (*hashMap)[i], *sucH = NULL;

        while(curH != NULL){
            sucH = curH->pn;

            nodeLetter *curL = curH->pl, *sucL = NULL;
            while(curL != NULL){
                sucL = curL->pn;
                free(curL);
                curL = sucL;
            }

            free(curH);
            curH = sucH;
        }
    }

    free(*hashMap);
}

void stampaFiltrate(nodeString *list) {
    while(list != NULL){
        if(list->valid)
            printf("%s\n", list->s);
        list = list->pn;
    }
}

void nuovaPartita(nodeString* list, int lengthBuff, int k){
    char* buffer = malloc(sizeof(char) * lengthBuff);
    int n;

    if(scanf("%[^\n]s", buffer) == EOF) exit(-3);
    while ((getchar()) != '\n');

    nodeString *pR = searchList(list, buffer);
    createHashMap(pR);

    if(scanf("%d\n", &n) == EOF) exit(-1);

    while(n > 0){
        if(scanf("%[^\n]s", buffer) == EOF) exit(-2);
        while ((getchar()) != '\n');

        if(strcmp(buffer, "+stampa_filtrate") == 0){
            stampaFiltrate(list);
            continue;
        }

        if(strcmp(buffer, "+inserisci_inizio") == 0){
            creazioneParole(&list, lengthBuff, k, "+inserisci_fine");
            continue;
        }

        if(strcmp(buffer, pR->s) == 0){
            printf("ok\n");
            break;
        }

        if(searchList(list, buffer) != NULL){
            n--;
            for(int i = 0; i < k; i++){
                if(buffer[i] == pR->s[i])
                    printf("+");
                else if(searchHashMapCar(&pR->ph, buffer[i]) == NULL)
                    printf("/");
                else{
                    nodeLetter* pL = searchHashMapCar(&pR->ph, buffer[i])->pl;
                    bool ok = false;

                    while(pL != NULL && ok == false){
                        if(pL->connected == false){
                            if(pR->s[pL->pos] == buffer[pL->pos])
                                pL->connected = true;
                            else{
                                pL->connected = true;
                                ok = true;
                            }
                        }
                        pL = pL->pn;
                    }

                    if(ok)
                        printf("|");
                    else
                        printf("/");
                }
            }
        }
        else
            printf("not exists\n");
    }

    freeHashMap(&pR->ph);
    free(buffer);
}

int main(){
    int k, lengthBuff;
    nodeString* list = NULL;

    if(scanf("%d\n", &k) == EOF) exit(-1);
    k++;
    setLengthBuff(&lengthBuff, k);

    creazioneParole(&list, lengthBuff, k, "+nuova_partita");
    nuovaPartita(list, lengthBuff, k);

    freeLista(&list);
    return 0;
}
