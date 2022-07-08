#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

const int HASHMAPSIZE = 7;

typedef enum{NESSUNO, MAGGIOREUGUALE, ESATTI} attributoLettere;

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

typedef struct nodePos{
    int pos;
    struct nodePos* pn;
}nodePos;

typedef struct nodeFilter{
    char c;
    int num;
    attributoLettere attr;
    struct nodeFilter* pn;
    nodePos* pG;
    nodePos* pS;
}nodeFilter;

typedef struct nodeCamb{
    nodeFilter *nodeF;
    struct nodeCamb *pn;
}nodeCamb;

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

// Cerca se esiste un carattere nell'hashMap e se esiste ritorna il suo puntatore
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
void insertHashMap(nodeHash*** hashMap, char car){
    nodeHash* headList = searchHashMapCar(hashMap, car);

    if(headList != NULL)
        headList->num++;
    else{
        nodeHash* ins = (nodeHash*) malloc(sizeof(nodeHash));

        ins->c = car;
        ins->pn = NULL;
        ins->num = 1;
        ins->pl = NULL;

        int cella = car % HASHMAPSIZE;
        ins->pn = (*hashMap)[cella];
        (*hashMap)[cella] = ins;
    }
}

void createHashMap(nodeString* s){
    s->ph = (nodeHash**) malloc(sizeof(nodeHash*) * HASHMAPSIZE);
    initializeHashMap(&s->ph);
    char* c = s->s;

    while(*c != '\0'){
        insertHashMap(&(s->ph), *c);
        c++;
    }
}

void createHashMapPos(nodeString* pS){
    for(int i = 0; pS->s[i] != '\0'; i++){
        nodeHash* pH = searchHashMapCar(&pS->ph, pS->s[i]);
        nodeLetter* pL = (nodeLetter*) malloc(sizeof(nodeLetter));
        pL->connected = false;
        pL->pos = i;
        pL->pn = pH->pl;
        pH->pl = pL;
    }
}

void restoreConnections(nodeHash ***hashMap) {
    for(int i = 0; i < HASHMAPSIZE; i++){
        nodeHash *pHash = (*hashMap)[i];
        while(pHash != NULL){
            nodeLetter *pLetter = pHash->pl;
            while(pLetter != NULL){
                pLetter->connected = false;
                pLetter = pLetter->pn;
            }
            pHash = pHash->pn;
        }
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
    createHashMap(ins);

    if(prev != NULL)
        prev->pn = ins;
    else
        *list = ins;
}

nodeString* searchListString(nodeString* list, char *buffer) {
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

void freeListaString(nodeString** list){
    nodeString *curS = *list, *sucS = NULL;

    while(curS != NULL){
        sucS = curS->pn;
        free(curS->s);

        for(int i = 0; i < HASHMAPSIZE; i++){
            nodeHash *curH = curS->ph[i], *sucH = NULL;

            while(curH != NULL){
                sucH = curH->pn;
                free(curH);
                curH = sucH;
            }
        }

        free(curS->ph);
        free(curS);
        curS = sucS;
    }

    *list = NULL;
}

void freeListaFiltro(nodeFilter **listFiltro) {
    nodeFilter *curF = *listFiltro, *sucF = NULL;

    while(curF != NULL){
        sucF = curF->pn;

        nodePos *curP = curF->pG, *sucP = NULL;
        while(curP != NULL){
            sucP = curP->pn;
            free(curP);
            curP = sucP;
        }

        curP = curF->pS, sucP = NULL;
        while(curP != NULL){
            sucP = curP->pn;
            free(curP);
            curP = sucP;
        }

        free(curF);
        curF = sucF;
    }

    *listFiltro = NULL;
}

void freeHashMapPos(nodeHash*** hashMap){
    for(int i = 0; i < HASHMAPSIZE; i++){
        nodeHash *curH = (*hashMap)[i];

        while(curH != NULL){
            nodeLetter *curL = curH->pl, *sucL = NULL;
            while(curL != NULL){
                sucL = curL->pn;
                free(curL);
                curL = sucL;
            }
            curH->pl = NULL;
            curH = curH->pn;
        }
    }
}

void stampaFiltrate(nodeString *list) {
    while(list != NULL){
        if(list->valid)
            printf("%s\n", list->s);
        list = list->pn;
    }
}

// Cerca se esiste un carattere nella lista di filtraggio e ritorna il suo puntatore oppure NULL
nodeFilter* searchListFilter(nodeFilter *list, char c) {
    while(list != NULL){
        if(list->c == c)
            return list;
        list = list->pn;
    }
    return NULL;
}

void letteraGiusta(nodeFilter **list, char c, int pos) {
    nodeFilter *pF = searchListFilter(*list, c);
    nodePos* pPos = malloc(sizeof(nodePos));
    pPos->pos = pos;

    if(pF == NULL){
        pF = malloc(sizeof(nodeFilter));
        pF->c = c;
        pF->num = 1;
        pF->attr = MAGGIOREUGUALE;
        pF->pS = NULL;

        pF->pG = pPos;
        pPos->pn = NULL;

        pF->pn = *list;
        *list = pF;
    }
    else{
        pF->num++;
        pPos->pn = pF->pG;
        pF->pG = pPos;
    }
}

void letteraInesistente(nodeFilter **list, char c) {
    nodeFilter *pF = searchListFilter(*list, c);

    if(pF == NULL){
        pF = malloc(sizeof(nodeFilter));
        pF->c = c;
        pF->num = 0;
        pF->attr = NESSUNO;
        pF->pG = NULL;
        pF->pS = NULL;

        pF->pn = *list;
        *list = pF;
    }

}

void letteraPosizioneSbagliata(nodeFilter **list, char c, int pos) {
    nodeFilter *pF = searchListFilter(*list, c);
    nodePos *pPos = malloc(sizeof(nodePos));
    pPos->pos = pos;

    if(pF == NULL){
        pF = malloc(sizeof(nodeFilter));
        pF->c = c;
        pF->num = 1;
        pF->attr = MAGGIOREUGUALE;
        pF->pG = NULL;

        pF->pS = pPos;
        pPos->pn = NULL;

        pF->pn = *list;
        *list = pF;
    }
    else{
        pF->num++;
        pPos->pn = pF->pS;
        pF->pS = pPos;
    }
}

void letteraConnessioniFinite(nodeFilter **list, char c, int pos) {
    nodeFilter *pF = searchListFilter(*list, c);
    nodePos *pPos = malloc(sizeof(nodePos));
    pPos->pos = pos;

    if(pF == NULL){
        pF = malloc(sizeof(nodeFilter));
        pF->c = c;
        pF->num = 0;
        pF->attr = ESATTI;
        pF->pG = NULL;

        pF->pS = pPos;
        pPos->pn = NULL;

        pF->pn = *list;
        *list = pF;
    }
    else{
        pF->attr = ESATTI;
        pPos->pn = pF->pS;
        pF->pS = pPos;
    }

}

void filtraStringhe(nodeString **listString, nodeFilter *listFilter) {
    while(listFilter != NULL){
        for(nodeString *head = *listString; head != NULL; head = head->pn){
            if(head->valid == false)
                continue;

            if(listFilter->attr == NESSUNO){
                if(searchHashMapCar(&head->ph, listFilter->c) != NULL)
                    head->valid = false;
                continue;
            }
            else{
                if(listFilter->attr == MAGGIOREUGUALE){
                    if(searchHashMapCar(&head->ph, listFilter->c) == NULL || searchHashMapCar(&head->ph, listFilter->c)->num < listFilter->num) {
                        head->valid = false;
                        continue;
                    }
                }
                else{
                    if(searchHashMapCar(&head->ph, listFilter->c) == NULL || searchHashMapCar(&head->ph, listFilter->c)->num != listFilter->num){
                        head->valid = false;
                        continue;
                    }

                }
                nodePos *pPos = listFilter->pG;

                while(pPos != NULL && head->valid == true){
                    if(head->s[pPos->pos] != listFilter->c)
                        head->valid = false;

                    pPos = pPos->pn;
                }

                pPos = listFilter->pS;

                while(pPos != NULL && head->valid == true){
                    if(head->s[pPos->pos] == listFilter->c)
                        head->valid = false;

                    pPos = pPos->pn;
                }
            }
        }
        listFilter = listFilter->pn;
    }
}

int numeroStringheValide(nodeString *listString) {
    int cont = 0;
    while(listString != NULL){
        if(listString->valid)
            cont++;
        listString = listString->pn;
    }
    return cont;
}

nodeFilter* searchListFiltroSto(nodeFilter *listFiltroSto, char c){
    while(listFiltroSto != NULL){
        if(listFiltroSto->c == c)
            return listFiltroSto;
        listFiltroSto = listFiltroSto->pn;
    }
    return NULL;
}

nodePos *searchNodePos(nodePos *list, int pos) {
    while(list != NULL){
        if(list->pos == pos)
            return list;
        list = list->pn;
    }
    return NULL;
}

void aggiornamentoFiltroStorico(nodeFilter *listFiltroRec, nodeFilter **listFiltroSto, nodeCamb **listCambiamenti) {
    while(listFiltroRec != NULL){
        nodeFilter *pFS = searchListFiltroSto(*listFiltroSto, listFiltroRec->c);

        // SE IL NODO NON ESISTE
        if(pFS == NULL){
            pFS = malloc(sizeof(nodeFilter));

            pFS->c = listFiltroRec->c;
            pFS->num = listFiltroRec->num;
            pFS->attr = listFiltroRec->attr;
            pFS->pG = NULL;
            pFS->pS = NULL;

            nodePos *nPR = listFiltroRec->pG; // TESTA DELLA LISTA RECENTE DEI NODI IN POSIZIONE GIUSTA

            while(nPR != NULL){
                nodePos *nPS = malloc(sizeof(nodePos));

                nPS->pos = nPR->pos;
                nPS->pn = pFS->pG;
                pFS->pG = nPS;

                nPR = nPR->pn;
            }

            nPR = listFiltroRec->pS; // TESTA DELLA LISTA RECENTE DEI NODI IN POSIZIONE SBAGLIATA

            while(nPR != NULL){
                nodePos *nPS = malloc(sizeof(nodePos));

                nPS->pos = nPR->pos;
                nPS->pn = pFS->pS;
                pFS->pS = nPS;

                nPR = nPR->pn;
            }

            pFS->pn = *listFiltroSto;
            *listFiltroSto = pFS;
        }
        else{
            // SE IL NODO ESISTE E L'ATTRIBUTO È NESSUNO NON FACCIO NIENTE
            // SE IL NODO ESISTE E L'ATTRIBUTO È MAGGIORE UGUALE O UGUALE DEVO VEDERE SE SONO CAMBIATI I NODI DI POSIZIONE E CONTROLLARE SE CAMBIANO ANCHE IL NUMERO DI LETTERE
            // SE IL NODO È UGUALE NON MODIFICO IN MAGGIORE UGUALE

            if(listFiltroRec->attr != NESSUNO){
                if(listFiltroRec->attr > pFS->attr)
                    pFS->attr = listFiltroRec->attr;

                nodePos *nPR = listFiltroRec->pG;

                while(nPR != NULL){
                    nodePos *nPS = searchNodePos(pFS->pG, nPR->pos);

                    if(nPS == NULL){
                        nPS = malloc(sizeof(nodePos));
                        nPS->pos = nPR->pos;
                        nPS->pn = pFS->pG;
                        pFS->pG = nPS;
                    }
                    nPR = nPR->pn;
                }

                nPR = listFiltroRec->pG;
                int cont = 0;

                while(nPR != NULL){
                    cont++;
                    nPR = nPR->pn;
                }

                // DEVO NOTIFICARE LA MODIFICA CON LA CREAZIONE DI UNA LISTA DI MODIFICHE
                if(cont > listFiltroRec->num){
                    listFiltroRec->num = cont;
                    nodeCamb *nM = malloc(sizeof(nodeCamb));
                    nM->nodeF = listFiltroRec;
                    nM->pn = *listCambiamenti;
                    *listCambiamenti = nM;
                }

                nPR = listFiltroRec->pS;

                while(nPR != NULL){
                    nodePos *nPS = searchNodePos(pFS->pS, nPR->pos);

                    if(nPS == NULL){
                        nPS = malloc(sizeof(nodePos));
                        nPS->pos = nPR->pos;
                        nPS->pn = pFS->pS;
                        pFS->pS = nPS;
                    }

                    nPR = nPR->pn;
                }
            }
        }

        listFiltroRec = listFiltroRec->pn;
    }
}

void filtraStringheCambiate(nodeString* listString, nodeCamb *listCamb) {
    while(listCamb != NULL){
        nodeFilter *pF = listCamb->nodeF;
        nodeCamb *listCambSuc = listCamb->pn;

        for(nodeString *headListString = listString; headListString != NULL; headListString = headListString->pn){
            if(headListString->valid == false)
                continue;
            else{
                if(pF->attr == MAGGIOREUGUALE){
                    if(searchHashMapCar(&headListString->ph, pF->c) == NULL || searchHashMapCar(&headListString->ph, pF->c)->num < pF->num) {
                        headListString->valid = false;
                        continue;
                    }
                }
                if(pF->attr == ESATTI){
                    if(searchHashMapCar(&headListString->ph, pF->c) == NULL || searchHashMapCar(&headListString->ph, pF->c)->num != pF->num){
                        headListString->valid = false;
                        continue;
                    }
                }
            }
        }

        free(listCamb);
        listCamb = listCambSuc;
    }
}

void restoreListStringValid(nodeString *listString) {
    while(listString != NULL){
        listString->valid = true;
        listString = listString->pn;
    }
}

void nuovaPartita(nodeString* listString, int lengthBuff, int k){
    char* buffer = malloc(sizeof(char) * lengthBuff);
    nodeFilter *listFiltroRec = NULL;
    nodeFilter *listFiltroSto = NULL;
    nodeCamb *listCambiamenti = NULL;
    int n;

    if(scanf("%[^\n]s", buffer) == EOF) exit(-3);
    while ((getchar()) != '\n');

    nodeString *pR = searchListString(listString, buffer);
    createHashMapPos(pR);

    if(scanf("%d\n", &n) == EOF) exit(-1);

    while(n > 0){
        if(scanf("%[^\n]s", buffer) == EOF) exit(-2);
        while ((getchar()) != '\n');

        if(strcmp(buffer, "+stampa_filtrate") == 0){
            stampaFiltrate(listString);
            continue;
        }

        if(strcmp(buffer, "+inserisci_inizio") == 0){
            creazioneParole(&listString, lengthBuff, k, "+inserisci_fine");
            filtraStringhe(&listString, listFiltroSto);
            continue;
        }

        if(strcmp(buffer, pR->s) == 0){
            printf("ok\n");
            break;
        }

        if(searchListString(listString, buffer) != NULL){
            n--;
            for(int i = 0; i < k - 1; i++){
                if(buffer[i] == pR->s[i]){
                    printf("+");
                    letteraGiusta(&listFiltroRec, buffer[i], i);
                }
                else if(searchHashMapCar(&pR->ph, buffer[i]) == NULL){
                    printf("/");
                    letteraInesistente(&listFiltroRec, buffer[i]);
                }
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

                    if(ok){
                        printf("|");
                        letteraPosizioneSbagliata(&listFiltroRec, buffer[i], i);
                    }
                    else{
                        printf("/");
                        letteraConnessioniFinite(&listFiltroRec, buffer[i], i);
                    }
                }
            }
            printf("\n");
            restoreConnections(&pR->ph);
            // Filtrare le stringhe
            filtraStringhe(&listString, listFiltroRec);
            // Applicare il filtro recente a quello storico
            aggiornamentoFiltroStorico(listFiltroRec, &listFiltroSto, &listCambiamenti);
            // Vedere se ci sono delle modifiche in quello storico rispetto al recente (numero di lettere)
            // Applicare solo le lettere modificate nello storico (salvate in una lista apposita)
            if(listCambiamenti != NULL){
                filtraStringheCambiate(listString, listCambiamenti);
                listCambiamenti = NULL;
            }

            printf("%d\n", numeroStringheValide(listString));
            freeListaFiltro(&listFiltroRec);
        }
        else
            printf("not_exists\n");

        if(n == 0)
            printf("ko\n");
    }

    restoreListStringValid(listString);
    freeHashMapPos(&pR->ph);
    freeListaFiltro(&listFiltroSto);
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

    char* buffer = malloc(sizeof(char) * lengthBuff);

    do{
        if(scanf("%[^\n]s", buffer) == EOF) break;
        while ((getchar()) != '\n');

        if(strcmp(buffer, "+inserisci_inizio") == 0)
            creazioneParole(&list, lengthBuff, k, "+inserisci_fine");
        else if(strcmp(buffer, "+nuova_partita") == 0)
            nuovaPartita(list, lengthBuff, k);

    } while (1);

    free(buffer);
    freeListaString(&list);
    return 0;
}