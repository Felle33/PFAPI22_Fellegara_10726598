#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

const int HASHMAPSIZE = 5;

typedef enum{NESSUNO, MAGGIOREUGUALE, ESATTI} attributoLettere;
typedef enum {RED, BLACK} color;

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

typedef struct tree{
    char *string;
    struct tree* left;
    struct tree* right;
    struct tree* parent;
    bool valid;
    nodeHash** ph;
    color col;
}nodeTree;

typedef struct{
    int size;
    int top;
    nodeTree **S;
} stack;

void printNode(nodeTree* head){
    printf("La chiave all'indirizzo %p e': %s (%d) e il suo padre e': %p (%s)\n", head, head->string, head->col, head->parent,
           head->parent != NULL ? head->parent->string : '\0');
}

void inizializeStack(stack *st){
    st->size = 5000;
    st->top = -1;
    st->S = malloc(sizeof(nodeTree *) * st->size);
}

bool stack_empty(stack* st){
    return st->top == -1 ? true : false;
}

void stack_addSize(stack *st) {
    st->size = st->size + st->size;
    if(realloc(st->S, st->size * sizeof(nodeTree *)) == NULL)
        exit(-1);
}

void stack_push(stack* st, nodeTree *val){
    if(st->top + 1 == st->size)
        stack_addSize(st);
    st->top++;
    st->S[st->top] = val;
}

nodeTree *stack_pop(stack* st){
    if(st->top == -1)
        return NULL;
    st->top--;
    return st->S[st->top + 1];
}

void inOrderTraverseTree(nodeTree* head){
    stack s;
    inizializeStack(&s);
    nodeTree *curr = head;

    while(curr != NULL || stack_empty(&s) == false){
        while(curr != NULL){
            stack_push(&s, curr);
            curr = curr->left;
        }
        curr = stack_pop(&s);
        printNode(curr);
        curr = curr->right;
    }

    free(s.S);
}

void inOrderTraverseTreeValid(nodeTree* head){
    stack s;
    inizializeStack(&s);
    nodeTree *curr = head;

    while(curr != NULL || stack_empty(&s) == false){
        while(curr != NULL){
            stack_push(&s, curr);
            curr = curr->left;
        }
        curr = stack_pop(&s);
        if(curr->valid)
            printf("%s\n",curr->string);
        curr = curr->right;
    }

    free(s.S);
}

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

void createHashMap(nodeTree *z){
    z->ph = (nodeHash**) malloc(sizeof(nodeHash*) * HASHMAPSIZE);
    initializeHashMap(&z->ph);
    char* c = z->string;

    while(*c != '\0'){
        insertHashMap(&(z->ph), *c);
        c++;
    }
}

nodeTree *searchBST(nodeTree* head, char* string){
    while(head != NULL){
        if(strcmp(string, head->string) == 0)
            return head;
        else if(strcmp(string, head->string) < 0)
            head = head->left;
        else
            head = head->right;
    }
    return head;
}

// Ruota il nodo x a sinistra
void leftRotate(nodeTree **root, nodeTree* x){
    nodeTree *y = x->right;
    x->right = y->left;

    if(y->left != NULL)
        y->left->parent = x;

    y->parent = x->parent;

    if(x->parent == NULL)
        *root = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

// Ruota il nodo x a destra
void rightRotate(nodeTree **root, nodeTree* x){
    nodeTree *y = x->left;
    x->left = y->right;

    if(y->right != NULL)
        y->right->parent = x;

    y->parent = x->parent;

    if(x->parent == NULL)
        *root = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->right = x;
    x->parent = y;
}

nodeTree* createTreeNode(nodeTree* parent, char *string, int k){
    nodeTree* result = malloc(sizeof(nodeTree));
    if(result != NULL){
        result->string = malloc(sizeof(char) * k);
        strcpy(result->string, string);
        result->left = NULL;
        result->right = NULL;
        result->parent = parent;
        result->valid = true;
        result->ph = NULL;
        createHashMap(result);
        result->col = RED;
    }
    return result;
}

void RBInsertFixup(nodeTree **root, nodeTree *z) {
    if(z == *root)
        (*root)->col = BLACK;
    else{
        nodeTree *x = z->parent; // x è il padre di z
        if(x->col == RED){
            if(x == x->parent->left){
                nodeTree *y = x->parent->right; // y è lo zio di z (fratello di x)

                if(y == NULL || y->col == BLACK){
                    if(z == x->left){
                        x->col = BLACK;
                        x->parent->col = RED;
                        rightRotate(root, x->parent);
                    }
                    else{
                        leftRotate(root, z->parent);
                        z->col = BLACK;
                        z->parent->col = RED;
                        rightRotate(root, z->parent);
                    }
                }
                else{
                    x->col = BLACK;
                    y->col = BLACK;
                    x->parent->col = RED;
                    RBInsertFixup(root, x->parent);
                }

            }
            else{
                nodeTree *y = x->parent->left; // y è lo zio di z (fratello di x)

                if(y == NULL || y->col == BLACK){
                    if(z == x->left){
                        rightRotate(root, z->parent);
                        z->col = BLACK;
                        z->parent->col = RED;
                        leftRotate(root, z->parent);
                    }
                    else{
                        x->col = BLACK;
                        x->parent->col = RED;
                        leftRotate(root, x->parent);
                    }
                }
                else{
                    x->col = BLACK;
                    y->col = BLACK;
                    x->parent->col = RED;
                    RBInsertFixup(root, x->parent);
                }
            }
        }
    }
}

void insertTree(nodeTree** head, char *string, int k){
    nodeTree* y = NULL;
    nodeTree* x = *head;

    while(x != NULL){
        y = x;
        if(strcmp(string, x->string) < 0)
            x = x->left;
        else
            x = x->right;
    }

    nodeTree* z = createTreeNode(y, string, k);

    if(y == NULL)
        *head = z;
    else if (strcmp(z->string, y->string) < 0)
        y->left = z;
    else
        y->right = z;

    RBInsertFixup(head, z);
}

void createHashMapPos(nodeTree *pS){
    for(int i = 0; pS->string[i] != '\0'; i++){
        nodeHash* pH = searchHashMapCar(&pS->ph, pS->string[i]);
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

void creazioneParole(nodeTree **tree, int lengthBuff, int k, char *endString){
    char* buffer = malloc(sizeof(char) * lengthBuff);

    while(1){
        if(scanf("%[^\n]s", buffer) == EOF) exit(-2);
        while ((getchar()) != '\n');
        if(strcmp(buffer, endString) == 0) break;
        insertTree(tree, buffer, k);
    }

    free(buffer);
}

void deleteAllTree(nodeTree *root) {
    stack s;
    inizializeStack(&s);
    nodeTree *curr = root;

    while(curr != NULL || stack_empty(&s) == false){
        while(curr != NULL){
            stack_push(&s, curr);
            curr = curr->left;
        }
        curr = stack_pop(&s);
        nodeTree *del = curr;

        for(int i = 0; i < HASHMAPSIZE; i++){
            nodeHash *curH = del->ph[i], *sucH = NULL;
            while(curH != NULL){
                sucH = curH->pn;
                free(curH);
                curH = sucH;
            }
        }

        curr = curr->right;
        free(del->ph);
        free(del->string);
        free(del);
    }

    free(s.S);
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

nodeTree* treeMinimum(nodeTree* head){
    while(head->left != NULL)
        head = head->left;
    return head;
}

nodeTree* treeSuccessor(nodeTree* head, char* string){
    nodeTree* x = searchBST(head, string);
    if(x->right != NULL)
        return treeMinimum(x->right);

    nodeTree* y = x->parent;
    while(y != NULL && x == y->right){
        x = y;
        y = y->parent;
    }
    return y;
}

void deleteTreeNode(nodeTree **head, nodeTree *del) {
    nodeTree *z = del;
    nodeTree *y = NULL, *x = NULL;

    if(z->left == NULL || z->right == NULL)
        y = z;
    else
        y = treeSuccessor(*head, z->string);

    if(y->left != NULL)
        x = y->left;
    else
        x = y->right;

    if(x != NULL)
        x->parent = y->parent;

    if(y->parent == NULL)
        *head = x;
    else if(y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;

    if(y != z)
        strcpy(z->string, y->string);
}

void filtraStringhe(nodeTree **treeStringValid, nodeFilter *listFilter) {
    while(listFilter != NULL){
        stack s;
        inizializeStack(&s);
        nodeTree *curr = *treeStringValid, *succ = NULL;

        for(; curr != NULL || stack_empty(&s) == false; curr = succ){
            while(curr != NULL){
                stack_push(&s, curr);
                curr = curr->left;
            }
            curr = stack_pop(&s);
            succ = curr->right;

            if(curr->valid == false)
                continue;

            if(listFilter->attr == NESSUNO){
                if(searchHashMapCar(&curr->ph, listFilter->c) != NULL)
                    curr->valid = false;//deleteTreeNode(treeStringValid, curr);
                continue;
            }
            else{
                if(listFilter->attr == MAGGIOREUGUALE){
                    if(searchHashMapCar(&curr->ph, listFilter->c) == NULL || searchHashMapCar(&curr->ph, listFilter->c)->num < listFilter->num) {
                        curr->valid = false;//deleteTreeNode(treeStringValid, curr);
                        continue;
                    }
                }
                else{
                    if(searchHashMapCar(&curr->ph, listFilter->c) == NULL || searchHashMapCar(&curr->ph, listFilter->c)->num != listFilter->num){
                        curr->valid = false;//deleteTreeNode(treeStringValid, curr);
                        continue;
                    }

                }
                nodePos *pPos = listFilter->pG;

                while(pPos != NULL && curr->valid == true){
                    if(curr->string[pPos->pos] != listFilter->c){
                        curr->valid = false;//deleteTreeNode(treeStringValid, curr);
                        break;
                    }

                    pPos = pPos->pn;
                }

                pPos = listFilter->pS;

                while(pPos != NULL && curr->valid == true){
                    if(curr->string[pPos->pos] == listFilter->c){
                        curr->valid = false;//deleteTreeNode(treeStringValid, curr);
                        break;
                    }

                    pPos = pPos->pn;
                }
            }
        }

        free(s.S);
        listFilter = listFilter->pn;
    }
}

int numeroStringheValide(nodeTree *treeStringValid) {
    int cont = 0;
    stack s;
    inizializeStack(&s);
    nodeTree *curr = treeStringValid;

    while(curr != NULL || stack_empty(&s) == false){
        while(curr != NULL){
            stack_push(&s, curr);
            curr = curr->left;
        }
        curr = stack_pop(&s);
        if(curr->valid == true)
            cont++;
        curr = curr->right;
    }
    free(s.S);
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

void filtraStringheCambiate(nodeTree **treeStringValid, nodeCamb *listCamb) {
    while(listCamb != NULL){
        nodeFilter *pF = listCamb->nodeF;
        nodeCamb *listCambSuc = listCamb->pn;

        stack s;
        inizializeStack(&s);
        nodeTree *curr = *treeStringValid, *succ = NULL;

        for(;curr != NULL || stack_empty(&s) == false; curr = succ){
            while(curr != NULL){
                stack_push(&s, curr);
                curr = curr->left;
            }
            curr = stack_pop(&s);
            succ = curr->right;

            if(curr->valid == false)
                continue;

            if(pF->attr == MAGGIOREUGUALE){
                if(searchHashMapCar(&curr->ph, pF->c) == NULL || searchHashMapCar(&curr->ph, pF->c)->num < pF->num) {
                    curr->valid = false;//deleteTreeNode(treeStringValid, curr);
                    continue;
                }
            }
            if(pF->attr == ESATTI){
                if(searchHashMapCar(&curr->ph, pF->c) == NULL || searchHashMapCar(&curr->ph, pF->c)->num != pF->num){
                    curr->valid = false;//deleteTreeNode(treeStringValid, curr);
                    continue;
                }
            }
        }

        free(s.S);
        free(listCamb);
        listCamb = listCambSuc;
    }
}

nodeTree *createTreeValidRec(nodeTree *treeString, nodeTree *parent, int k){
    if(treeString == NULL)
        return NULL;
    nodeTree *newNode = createTreeNode(parent, treeString->string, k);
    newNode->left = createTreeValidRec(treeString->left, newNode, k);
    newNode->right = createTreeValidRec(treeString->right, newNode, k);
    return newNode;
}

void restoreTreeStringValid(nodeTree *treeString) {
    stack s;
    inizializeStack(&s);
    nodeTree *curr = treeString;

    while(curr != NULL || stack_empty(&s) == false){
        while(curr != NULL){
            stack_push(&s, curr);
            curr = curr->left;
        }
        curr = stack_pop(&s);
        curr->valid = true;
        curr = curr->right;
    }

    free(s.S);
}

void nuovaPartita(nodeTree **treeString, int lengthBuff, int k){
    char* buffer = malloc(sizeof(char) * lengthBuff);
    //nodeTree *treeStringValid = NULL;
    nodeFilter *listFiltroRec = NULL;
    nodeFilter *listFiltroSto = NULL;
    nodeCamb *listCambiamenti = NULL;
    int n;

    if(scanf("%[^\n]s", buffer) == EOF) exit(-3);
    while ((getchar()) != '\n');

    nodeTree *pR = searchBST(*treeString, buffer);
    createHashMapPos(pR);
    //treeStringValid = createTreeValidRec(*treeString, NULL, k);

    if(scanf("%d\n", &n) == EOF) exit(-1);

    while(n > 0){
        if(scanf("%[^\n]s", buffer) == EOF) exit(-2);
        while ((getchar()) != '\n');

        if(strcmp(buffer, "+stampa_filtrate") == 0){
            inOrderTraverseTreeValid(*treeString);
            continue;
        }

        if(strcmp(buffer, "+inserisci_inizio") == 0){
            creazioneParole(treeString, lengthBuff, k, "+inserisci_fine");
            //deleteAllTree(treeStringValid);
            //treeStringValid = createTreeValidRec(*treeString, NULL, k);
            filtraStringhe(treeString, listFiltroSto);
            continue;
        }

        if(strcmp(buffer, pR->string) == 0){
            printf("ok\n");
            break;
        }

        if(searchBST(*treeString, buffer) != NULL){
            n--;
            for(int i = 0; i < k - 1; i++){
                if(buffer[i] == pR->string[i]){
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
                            if(pR->string[pL->pos] == buffer[pL->pos])
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
            filtraStringhe(treeString, listFiltroRec);
            // Applicare il filtro recente a quello storico
            aggiornamentoFiltroStorico(listFiltroRec, &listFiltroSto, &listCambiamenti);
            // Vedere se ci sono delle modifiche in quello storico rispetto al recente (numero di lettere)
            // Applicare solo le lettere modificate nello storico (salvate in una lista apposita)
            if(listCambiamenti != NULL){
                filtraStringheCambiate(treeString, listCambiamenti);
                listCambiamenti = NULL;
            }

            printf("%d\n", numeroStringheValide(*treeString));
            freeListaFiltro(&listFiltroRec);
        }
        else
            printf("not_exists\n");

        if(n == 0)
            printf("ko\n");
    }

    //inOrderTraverseTree(*treeString);
    //deleteAllTree(treeStringValid);
    restoreTreeStringValid(*treeString);
    freeHashMapPos(&pR->ph);
    freeListaFiltro(&listFiltroSto);
    free(buffer);
}

int main(){
    int k, lengthBuff;
    //nodeString* list = NULL;
    nodeTree* treeString = NULL;


    if(scanf("%d\n", &k) == EOF) exit(-1);
    k++;
    setLengthBuff(&lengthBuff, k);

    creazioneParole(&treeString, lengthBuff, k, "+nuova_partita");
    nuovaPartita(&treeString, lengthBuff, k);

    char* buffer = malloc(sizeof(char) * lengthBuff);

    do{
        if(scanf("%[^\n]s", buffer) == EOF) break;
        while ((getchar()) != '\n');

        if(strcmp(buffer, "+inserisci_inizio") == 0){
            creazioneParole(&treeString, lengthBuff, k, "+inserisci_fine");
        }
        else if(strcmp(buffer, "+nuova_partita") == 0)
            nuovaPartita(&treeString, lengthBuff, k);

    } while (1);

    free(buffer);
    deleteAllTree(treeString);
    return 0;
}