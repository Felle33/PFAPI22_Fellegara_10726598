#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum{NESSUNO, MAGGIOREUGUALE, ESATTI} attributoLettere;
typedef enum {RED, BLACK} color;

const int numberInput = 100;

typedef struct nodeList{
    char *string;
    struct nodeList* pn;
}nodeList;

typedef struct tree{
    char *string;
    struct tree* left;
    struct tree* right;
    struct tree* parent;
    nodeList* pList;
    color col;
}nodeTree;

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

void setLengthBuff(int *lengthBuff, int k){
    if(k > 17)
        *lengthBuff = k + 1;
    else
        *lengthBuff = 18;
}

nodeTree* createTreeNode(nodeTree* parent, char *string, int k, color col){
    nodeTree* result = malloc(sizeof(nodeTree));
    if(result != NULL){
        result->string = malloc(sizeof(char) * k);
        strcpy(result->string, string);
        result->left = NULL;
        result->right = NULL;
        result->parent = parent;
        result->pList = NULL;
        result->col = col;
    }
    return result;
}

nodeList* createNodeList(char *string, int k){
    nodeList *node = malloc(sizeof(nodeList));
    node->string = malloc(sizeof(char) * k);
    strcpy(node->string, string);
    node->pn = NULL;
    return node;
}

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

    nodeTree* z = createTreeNode(y, string, k, RED);

    if(y == NULL)
        *head = z;
    else if (strcmp(z->string, y->string) < 0)
        y->left = z;
    else
        y->right = z;

    RBInsertFixup(head, z);
}

nodeTree* searchBST(nodeTree* head, char* string){
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

nodeTree* treeMinimum(nodeTree* head){
    while(head->left != NULL)
        head = head->left;
    return head;
}

nodeTree* treeMaximum(nodeTree* head){
    while(head->right != NULL)
        head = head->right;
    return head;
}

nodeTree* treeSuccessor(nodeTree* treeString, nodeTree* x){
    if(x->right != NULL)
        return treeMinimum(x->right);

    nodeTree* y = x->parent;
    while(y != NULL && x == y->right){
        x = y;
        y = y->parent;
    }
    return y;
}

nodeTree *treePredecessor(nodeTree *treeString, nodeTree *x) {
    if(x->left != NULL)
        return treeMaximum(x->left);

    nodeTree* y = x->parent;

    while(y != NULL && x == y->left){
        x = y;
        y = y->parent;
    }
    return y;
}

void creazioneParole(nodeTree **tree, int lengthBuff, int k, char *endString){
    char *buffer = malloc(sizeof(char) * lengthBuff);

    while(1){
        if(scanf("%[^\n]s", buffer) == EOF) exit(-2);
        while ((getchar()) != '\n');
        if(strcmp(buffer, endString) == 0) break;
        insertTree(tree, buffer, k);

    }

    free(buffer);
}

void deleteAllTree(nodeTree *root) {
    if(root == NULL) return;
    deleteAllTree(root->left);
    deleteAllTree(root->right);
    free(root->string);
    free(root);
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

void inOrderTraverseListValid(nodeList *listStringValid){
    while(listStringValid != NULL){
        printf("%s\n", listStringValid->string);
        listStringValid = listStringValid->pn;
    }
}

int numeroStringheValide(nodeList *listStringValid) {
    int cont = 0;
    while(listStringValid != NULL){
        cont++;
        listStringValid = listStringValid->pn;
    }
    return cont;
}

bool controlloPosizioni(nodeList *curr, nodeFilter *nodeFil){
    bool delete = false;
    nodePos *pPos = nodeFil->pG;

    while(pPos != NULL && delete == false){
        if(curr->string[pPos->pos] != nodeFil->c)
            delete = true;//deleteTreeNode(treeStringValid, curr);

        pPos = pPos->pn;
    }

    pPos = nodeFil->pS;

    while(pPos != NULL && delete == false) {
        if (curr->string[pPos->pos] == nodeFil->c)
            delete = true;//deleteTreeNode(treeStringValid, curr);

        pPos = pPos->pn;
    }

    return delete;
}

void filtraNessuno(nodeTree *treeString, nodeList **listStringValid, char c, int k){
    nodeList *curr = *listStringValid, *prev = NULL, *succ = NULL;

    while(curr != NULL){
        succ = curr->pn;
        for(int i = 0; i < k - 1; i++)
            if(curr->string[i] == c){
                if(prev == NULL)
                    *listStringValid = curr->pn;
                else
                    prev->pn = curr->pn;

                searchBST(treeString, curr->string)->pList = NULL;
                free(curr->string);
                free(curr);
                curr = prev;
                break;
            }
        prev = curr;
        curr = succ;
    }
}

void filtraMaggioreUguale(nodeTree *treeString, nodeList **listStringValid, nodeFilter *nodeFilt, int k){
    nodeList *curr = *listStringValid, *prev = NULL, *succ = NULL;

    while(curr != NULL){
        succ = curr->pn;
        if(controlloPosizioni(curr, nodeFilt) == true){
            if(prev == NULL)
                *listStringValid = curr->pn;
            else
                prev->pn = curr->pn;

            searchBST(treeString, curr->string)->pList = NULL;
            free(curr->string);
            free(curr);
            curr = prev;
        }
        else{
            int cont = 0;
            for(int i = 0; i < k - 1 && cont < nodeFilt->num; i++)
                if(curr->string[i] == nodeFilt->c)
                    cont++;
            if(cont < nodeFilt->num){
                if(prev == NULL)
                    *listStringValid = curr->pn;
                else
                    prev->pn = curr->pn;

                searchBST(treeString, curr->string)->pList = NULL;
                free(curr->string);
                free(curr);
                curr = prev;
            }
        }
        prev = curr;
        curr = succ;
    }
}

void filtraEsatti(nodeTree *treeString, nodeList **listStringValid, nodeFilter *nodeFilt, int k){
    nodeList *curr = *listStringValid, *prev = NULL, *succ = NULL;

    while(curr != NULL){
        succ = curr->pn;
        if(controlloPosizioni(curr, nodeFilt) == true){
            if(prev == NULL)
                *listStringValid = curr->pn;
            else
                prev->pn = curr->pn;

            searchBST(treeString, curr->string)->pList = NULL;
            free(curr->string);
            free(curr);
            curr = prev;
        }
        else{
            int cont = 0;
            for(int i = 0; i < k - 1; i++)
                if(curr->string[i] == nodeFilt->c)
                    cont++;
            if(cont != nodeFilt->num){
                if(prev == NULL)
                    *listStringValid = curr->pn;
                else
                    prev->pn = curr->pn;

                searchBST(treeString, curr->string)->pList = NULL;
                free(curr->string);
                free(curr);
                curr = prev;
            }
        }
        prev = curr;
        curr = succ;
    }
}

void filtraStringhe(nodeTree *treeString, nodeList **listStringValid, nodeFilter *listFilter, int k) {
    while(listFilter != NULL){
        if(listFilter->attr == NESSUNO)
            filtraNessuno(treeString, listStringValid, listFilter->c, k);
        else if(listFilter->attr == MAGGIOREUGUALE)
            filtraMaggioreUguale(treeString, listStringValid, listFilter, k);
        else
            filtraEsatti(treeString, listStringValid, listFilter, k);
        listFilter = listFilter->pn;
    }
}

nodeFilter* searchListFiltroSto(nodeFilter *listFiltroSto, char c){
    while(listFiltroSto != NULL){
        if(listFiltroSto->c == c)
            return listFiltroSto;
        listFiltroSto = listFiltroSto->pn;
    }
    return NULL;
}

nodePos* searchNodePos(nodePos *list, int pos) {
    while(list != NULL){
        if(list->pos == pos)
            return list;
        list = list->pn;
    }
    return NULL;
}

void aggiornamentoFiltroStorico(nodeFilter *listFiltroRec, nodeFilter **listFiltroSto) {
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
                int cont = 0;

                while(nPR != NULL){
                    cont++;
                    nodePos *nPS = searchNodePos(pFS->pG, nPR->pos);

                    if(nPS == NULL){
                        nPS = malloc(sizeof(nodePos));
                        nPS->pos = nPR->pos;
                        nPS->pn = pFS->pG;
                        pFS->pG = nPS;
                    }
                    nPR = nPR->pn;
                }

                // DEVO NOTIFICARE L'AGGIORNAMENTO MODIFICANDO LA LISTA RECENTE PER POI APPLICARLA
                if(cont > listFiltroRec->num)
                    listFiltroRec->num = cont;

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

bool filtraParola(char *string, nodeFilter *listFiltroSto, int k){
    while(listFiltroSto != NULL){
        if(listFiltroSto->attr == NESSUNO){
            for(int i = 0; i < k - 1; i++)
                if(string[i] == listFiltroSto->c)
                    return false;
        }
        else{
            int cont = 0;
            if(listFiltroSto->attr == ESATTI){
                for(int i = 0; i < k - 1; i++)
                    if(string[i] == listFiltroSto->c)
                        cont++;
                if(cont != listFiltroSto->num)
                    return false;
            }
            else{
                for(int i = 0; i < k - 1 && cont < listFiltroSto->num; i++)
                    if(string[i] == listFiltroSto->c)
                        cont++;
                if(cont < listFiltroSto->num)
                    return false;
            }

            nodePos *nPS = listFiltroSto->pG;

            while(nPS != NULL){
                if(string[nPS->pos] != listFiltroSto->c)
                    return false;
                nPS = nPS->pn;
            }

            nPS = listFiltroSto->pS;

            while(nPS != NULL){
                if(string[nPS->pos] == listFiltroSto->c)
                    return false;
                nPS = nPS->pn;
            }
        }
        listFiltroSto = listFiltroSto->pn;
    }
    return true;
}

void inserisciInizio(nodeTree **treeString, nodeList **listStringValid, nodeFilter *listFiltroSto, int lengthBuff, int k) {
    char* buffer = malloc(sizeof(char) * lengthBuff);

    while(1){
        if(scanf("%[^\n]s", buffer) == EOF) exit(-2);
        while ((getchar()) != '\n');
        if(strcmp(buffer, "+inserisci_fine") == 0) break;
        insertTree(treeString, buffer, k);
        if(filtraParola(buffer, listFiltroSto, k) == true){
            nodeTree *curr = searchBST(*treeString, buffer);
            nodeTree *prev = curr;

            do{
                prev = treePredecessor(*treeString, prev);
            }
            while(prev != NULL && prev->pList == NULL);

            nodeList *nodeString = createNodeList(buffer, k);
            curr->pList = nodeString;

            if(prev == NULL){
                nodeString->pn = *listStringValid;
                *listStringValid = nodeString;
            }
            else{
                nodeString->pn = prev->pList->pn;
                prev->pList->pn = nodeString;
            }
        }
    }

    free(buffer);
}

nodeList* createListStringValidIt(nodeTree *treeString, int k){
    nodeTree *succ = treeMinimum(treeString);
    nodeList *head = createNodeList(succ->string, k);
    succ->pList = head;
    nodeList *curr = head;

    while((succ = treeSuccessor(treeString, succ)) != NULL){
        curr->pn = createNodeList(succ->string, k);
        succ->pList = curr->pn;
        curr = curr->pn;
    }

    return head;
}

void deleteListString(nodeList **list) {
    nodeList *curr = *list;
    while(curr != NULL){
        nodeList *succ = curr->pn;
        free(curr->string);
        free(curr);
        curr = succ;
    }
    *list = NULL;
}

void nuovaPartita(nodeTree **treeString, int lengthBuff, int k){
    char* buffer = malloc(sizeof(char) * lengthBuff);
    nodeList *listStringValid = NULL;
    nodeFilter *listFiltroRec = NULL;
    nodeFilter *listFiltroSto = NULL;
    int n;

    if(scanf("%[^\n]s", buffer) == EOF) exit(-3);
    while ((getchar()) != '\n');

    nodeTree *pR = searchBST(*treeString, buffer);
    listStringValid = createListStringValidIt(*treeString, k);

    if(scanf("%d\n", &n) == EOF) exit(-1);

    while(n > 0){
        if(scanf("%[^\n]s", buffer) == EOF) exit(-2);
        while ((getchar()) != '\n');

        if(strcmp(buffer, "+stampa_filtrate") == 0){
            inOrderTraverseListValid(listStringValid);
            continue;
        }

        if(strcmp(buffer, "+inserisci_inizio") == 0){
            inserisciInizio(treeString, &listStringValid, listFiltroSto, lengthBuff, k);
            //filtraStringhe(&listStringValid, listFiltroSto, k);
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
                else{
                    bool exist = false;
                    int lettereLibere = 0;

                    for(int j = 0; j < k - 1; j++){
                        if(buffer[i] == pR->string[j]){
                            exist = true;
                            if(pR->string[j] != buffer[j])
                                lettereLibere++;
                        }
                    }

                    if(exist == false){
                        printf("/");
                        letteraInesistente(&listFiltroRec, buffer[i]);
                    }
                    else{
                        int lettereNonInPosizioneCorretta = 0;
                        for(int j = 0; j <= i; j++){
                            if(buffer[i] == buffer[j] && buffer[j] != pR->string[j])
                                lettereNonInPosizioneCorretta++;
                        }

                        if(lettereNonInPosizioneCorretta <= lettereLibere){
                            printf("|");
                            letteraPosizioneSbagliata(&listFiltroRec, buffer[i], i);
                        }
                        else{
                            printf("/");
                            letteraConnessioniFinite(&listFiltroRec, buffer[i], i);
                        }
                    }
                }
            }
            printf("\n");
            // Applicare il filtro recente a quello storico
            // Vedere se ci sono delle modifiche in quello storico rispetto al recente (numero di lettere)
            // Se ci sono delle modifiche, aggiorno la lista recente e poi la applico
            aggiornamentoFiltroStorico(listFiltroRec, &listFiltroSto);
            // Filtrare le stringhe
            filtraStringhe(*treeString, &listStringValid, listFiltroRec, k);
            printf("%d\n", numeroStringheValide(listStringValid));
            freeListaFiltro(&listFiltroRec);
        }
        else
            printf("not_exists\n");

        if(n == 0)
            printf("ko\n");
    }

    deleteListString(&listStringValid);
    freeListaFiltro(&listFiltroSto);
    free(buffer);
}

int main() {
    int k, lengthBuff;
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

        if(strcmp(buffer, "+inserisci_inizio") == 0)
            creazioneParole(&treeString, lengthBuff, k, "+inserisci_fine");
        else if(strcmp(buffer, "+nuova_partita") == 0)
            nuovaPartita(&treeString, lengthBuff, k);

    } while (1);

    free(buffer);
    deleteAllTree(treeString);
    return 0;
}