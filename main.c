#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum{NESSUNO, MAGGIOREUGUALE, ESATTI} attributoLettere;
typedef enum {RED, BLACK} color;

int k;
int numeroParoleFiltrate = 0;

typedef struct treeString{
    char *string;
    struct treeString* left;
    struct treeString* right;
    struct treeString* parent;
    color col;
}nodeTreeString;

typedef struct nodePos{
    int pos;
    struct nodePos* next;
}nodePos;

typedef struct nodeFilter{
    char c;
    int num;
    attributoLettere attr;
    struct nodeFilter* next;
    nodePos* pG;
    nodePos* pS;
}nodeFilter;

typedef struct nodeCamb{
    nodeTreeString *nodeTreeStringCamb;
    struct nodeCamb* next;
}nodeCamb;

typedef struct nodeLetter{
    char c;
    int num;
    struct nodeLetter* next;
}nodeLetter;

int stringCmp(const char* s1, const char* s2);
void stringCpy(char* dest, const char* source);
nodeTreeString* searchBST(nodeTreeString* tree, nodeTreeString* NIL, char* string);
nodeTreeString* treeMinimum(nodeTreeString* node, nodeTreeString* NIL);
nodeTreeString* treeSuccessor(nodeTreeString* node, nodeTreeString* NIL);
nodeTreeString* createTreeNodeString(nodeTreeString *NIL, char *string);
nodeTreeString *createNIL();
void leftRotate(nodeTreeString **root, nodeTreeString* x, nodeTreeString* NIL);
void rightRotate(nodeTreeString **root, nodeTreeString* x, nodeTreeString* NIL);
void RBInsertFixup(nodeTreeString **root, nodeTreeString *z, nodeTreeString *NIL);
void insertTreeNode(nodeTreeString ** root, nodeTreeString *NIL, nodeTreeString *ins);
void RBDeleteFixup(nodeTreeString **root, nodeTreeString *x, nodeTreeString *NIL);
nodeTreeString *moveNode(nodeTreeString **source, nodeTreeString **dest, nodeTreeString *NIL, nodeTreeString *z);
void deleteTree(nodeTreeString *treeStringValid,  nodeTreeString *NIL, nodeCamb **listCamb);
void creazioneParole(nodeTreeString **treeStringAll, nodeTreeString *NIL, int lengthBuff, const char *endString);
nodeFilter* searchListFilter(nodeFilter *list, char c);
void deleteListFilter(nodeFilter **listFilter);
nodePos* searchNodePos(nodePos *listNodePos, int pos);
void inOrderTraverseTreeValid(nodeTreeString *treeStringValid, nodeTreeString *NIL);
void filtroLettera(nodeFilter *listFilterSto, nodeFilter **listFilterRec, char c, int pos, int id);
bool filtraParola(const char *string, nodeFilter *listFilter);
void aggiornamentoFiltroStorico(nodeFilter **listFilterRec, nodeFilter **listFilterSto);
void inserisciInizio(nodeTreeString **treeStringValid, nodeTreeString **treeStringAll, nodeTreeString *NIL, nodeFilter *listFilterSto, int lengthBuff);
bool controlloPosizioni(const char *string, nodeFilter *nodeFil);
void filtraStringheFromAll(nodeTreeString *treeString, nodeTreeString *NIL, nodeFilter *listFilter, nodeCamb **listCamb, bool filPar);
void nuovaPartita(nodeTreeString **treeStringAll, nodeTreeString *NIL, int lengthBuff);
void insertListCamb(nodeCamb **listCamb, nodeTreeString *treeString);
void modifyValidTree(nodeTreeString **treeStringAll, nodeTreeString **treeStringValid, nodeTreeString *NIL, nodeCamb **listCamb, bool allToValid);

int stringCmp(const char* s1, const char* s2){
    while (*s1 && (*s1 == *s2))
        s1++, s2++;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void stringCpy(char* dest, const char* source){
    for(int i = 0; i < k; i++)
        dest[i] = source[i];
}

nodeTreeString* searchBST(nodeTreeString* tree, nodeTreeString* NIL, char* string){
    while(tree != NIL){
        int n = stringCmp(string, tree->string);
        if(n == 0)
            return tree;
        else if(n < 0)
            tree = tree->left;
        else
            tree = tree->right;
    }
    return tree;
}

nodeTreeString* treeMinimum(nodeTreeString* node, nodeTreeString* NIL){
    while(node->left != NIL)
        node = node->left;
    return node;
}

nodeTreeString* treeSuccessor(nodeTreeString* node, nodeTreeString* NIL){
    if(node->right != NIL)
        return treeMinimum(node->right, NIL);

    nodeTreeString* y = node->parent;
    while(y != NIL && node == y->right){
        node = y;
        y = y->parent;
    }
    return y;
}

void leftRotate(nodeTreeString **root, nodeTreeString* x, nodeTreeString* NIL){
    nodeTreeString *y = x->right;
    x->right = y->left;

    if(y->left != NIL)
        y->left->parent = x;

    y->parent = x->parent;

    if(x->parent == NIL)
        *root = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void rightRotate(nodeTreeString **root, nodeTreeString* x, nodeTreeString* NIL){
    nodeTreeString *y = x->left;
    x->left = y->right;

    if(y->right != NIL)
        y->right->parent = x;

    y->parent = x->parent;

    if(x->parent == NIL)
        *root = y;
    else if(x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->right = x;
    x->parent = y;
}

void RBInsertFixup(nodeTreeString **root, nodeTreeString *z, nodeTreeString *NIL) {
    if(z == *root)
        (*root)->col = BLACK;
    else{
        nodeTreeString *x = z->parent;
        if(x->col == RED){
            if(x == x->parent->left){
                nodeTreeString *y = x->parent->right;
                if(y->col == RED){
                    x->col = BLACK;
                    y->col = BLACK;
                    x->parent->col = RED;
                    RBInsertFixup(root, x->parent, NIL);
                }
                else{
                    if(z == x->right){
                        z = x;
                        leftRotate(root, z, NIL);
                        x = z->parent;
                    }
                    x->col = BLACK;
                    x->parent->col = RED;
                    rightRotate(root, x->parent, NIL);
                }
            }
            else{
                nodeTreeString *y = x->parent->left;
                if(y->col == RED){
                    x->col = BLACK;
                    y->col = BLACK;
                    x->parent->col = RED;
                    RBInsertFixup(root, x->parent, NIL);
                }
                else{
                    if(z == x->left){
                        z = x;
                        rightRotate(root, z, NIL);
                        x = z->parent;
                    }
                    x->col = BLACK;
                    x->parent->col = RED;
                    leftRotate(root, x->parent, NIL);
                }
            }
        }
    }
}

nodeTreeString* createTreeNodeString(nodeTreeString *NIL, char *string){
    nodeTreeString* result = malloc(sizeof(nodeTreeString));
    if(result != NULL){
        result->string = malloc(sizeof(char) * k);
        stringCpy(result->string, string);
        result->left = NIL;
        result->right = NIL;
        result->parent = NIL;
        result->col = RED;
    }
    return result;
}

void insertTreeNode(nodeTreeString** root, nodeTreeString *NIL, nodeTreeString *ins){
    nodeTreeString* y = NIL, * x = *root;

    while(x != NIL){
        y = x;
        if(stringCmp(ins->string, x->string) < 0)
            x = x->left;
        else
            x = x->right;
    }

    ins->parent = y;

    if(y == NIL)
        *root = ins;
    else if (stringCmp(ins->string, y->string) < 0)
        y->left = ins;
    else
        y->right = ins;
    RBInsertFixup(root, ins, NIL);
}

void creazioneParole(nodeTreeString **treeStringAll, nodeTreeString *NIL, int lengthBuff, const char *endString){
    char buffer[lengthBuff];

    while(1){
        if(fgets(buffer, lengthBuff, stdin) == NULL) exit(-2);
        if(stringCmp(buffer, endString) == 0) break;
        buffer[k - 1] = '\0';
        insertTreeNode(treeStringAll, NIL, createTreeNodeString(NIL, buffer));
    }
}

nodePos* searchNodePos(nodePos *listNodePos, int pos) {
    while(listNodePos != NULL){
        if(listNodePos->pos == pos)
            return listNodePos;
        listNodePos = listNodePos->next;
    }
    return NULL;
}

nodeFilter* searchListFilter(nodeFilter *list, char c) {
    while(list != NULL){
        if(list->c == c)
            return list;
        list = list->next;
    }
    return NULL;
}

void inOrderTraverseTreeValid(nodeTreeString *treeStringValid, nodeTreeString *NIL){
    if(treeStringValid == NIL) return;
    inOrderTraverseTreeValid(treeStringValid->left, NIL);
    printf("%s\n", treeStringValid->string);
    inOrderTraverseTreeValid(treeStringValid->right, NIL);
}

nodeTreeString *createNIL() {
    nodeTreeString *z = malloc(sizeof(nodeTreeString));
    z->string = NULL;
    z->left = NULL;
    z->right = NULL;
    z->parent = NULL;
    z->col = BLACK;
    return z;
}

void filtroLettera(nodeFilter *listFilterSto, nodeFilter **listFilterRec, char c, int pos, int id){
    nodeFilter *pSto = searchListFilter(listFilterSto, c);
    nodeFilter *pRec = searchListFilter(*listFilterRec, c);

    switch (id) {
        case 1:
            if(pSto == NULL && pRec == NULL){
                pRec = malloc(sizeof(nodeFilter));
                pRec->c = c;
                pRec->num = 0;
                pRec->attr = NESSUNO;
                pRec->pG = NULL;
                pRec->pS = NULL;

                pRec->next = *listFilterRec;
                *listFilterRec = pRec;
            }
            break;
        case 2:
            if(pRec == NULL){
                pRec = malloc(sizeof(nodeFilter));
                pRec->c = c;
                pRec->num = 1;
                pRec->attr = MAGGIOREUGUALE;
                pRec->pS = NULL;

                if(pSto == NULL || searchNodePos(pSto->pG, pos) == NULL){
                    nodePos *pPos = malloc(sizeof(nodePos));
                    pPos->pos = pos;
                    pRec->pG = pPos;
                    pPos->next = NULL;
                }
                else
                    pRec->pG = NULL;

                pRec->next = *listFilterRec;
                *listFilterRec = pRec;
            }
            else{
                pRec->num++;
                if(pSto == NULL || searchNodePos(pSto->pG, pos) == NULL){
                    nodePos *pPos = malloc(sizeof(nodePos));
                    pPos->pos = pos;
                    pPos->next = pRec->pG;
                    pRec->pG = pPos;
                }
            }
            break;
        case 3:
            if(pRec == NULL){
                pRec = malloc(sizeof(nodeFilter));
                pRec->c = c;
                pRec->num = 1;
                pRec->attr = MAGGIOREUGUALE;
                pRec->pG = NULL;

                if(pSto == NULL || searchNodePos(pSto->pS, pos) == NULL){
                    nodePos *pPos = malloc(sizeof(nodePos));
                    pPos->pos = pos;
                    pRec->pS = pPos;
                    pPos->next = NULL;
                }
                else
                    pRec->pS = NULL;

                pRec->next = *listFilterRec;
                *listFilterRec = pRec;
            }
            else{
                pRec->num++;
                if(pSto == NULL || searchNodePos(pSto->pS, pos) == NULL){
                    nodePos *pPos = malloc(sizeof(nodePos));
                    pPos->pos = pos;
                    pPos->next = pRec->pS;
                    pRec->pS = pPos;
                }
            }
            break;
        case 4:
            if(pRec == NULL){
                pRec = malloc(sizeof(nodeFilter));
                pRec->c = c;
                pRec->num = 0;
                pRec->attr = ESATTI;
                pRec->pG = NULL;

                if(pSto == NULL || searchNodePos(pSto->pS, pos) == NULL){
                    nodePos *pPos = malloc(sizeof(nodePos));
                    pPos->pos = pos;
                    pRec->pS = pPos;
                    pPos->next = NULL;
                }
                else
                    pRec->pS = NULL;

                pRec->next = *listFilterRec;
                *listFilterRec = pRec;
            }
            else{
                pRec->attr = ESATTI;
                if(pSto == NULL || searchNodePos(pSto->pS, pos) == NULL){
                    nodePos *pPos = malloc(sizeof(nodePos));
                    pPos->pos = pos;
                    pPos->next = pRec->pS;
                    pRec->pS = pPos;
                }
            }
            break;
        default:
            break;
    }
}

void aggiornamentoFiltroStorico(nodeFilter **listFilterRec, nodeFilter **listFilterSto) {
    nodeFilter *curr = *listFilterRec;
    while(curr != NULL) {
        nodeFilter *pFS = searchListFilter(*listFilterSto, curr->c);

        // SE IL NODO NON ESISTE
        if (pFS == NULL) {
            pFS = malloc(sizeof(nodeFilter));

            pFS->c = curr->c;
            pFS->num = curr->num;
            pFS->attr = curr->attr;
            pFS->pG = NULL;
            pFS->pS = NULL;

            nodePos *nPR = curr->pG; // TESTA DELLA LISTA RECENTE DEI NODI IN POSIZIONE GIUSTA

            while (nPR != NULL) {
                nodePos *nPS = malloc(sizeof(nodePos));

                nPS->pos = nPR->pos;
                nPS->next = pFS->pG;
                pFS->pG = nPS;

                nPR = nPR->next;
            }

            nPR = curr->pS; // TESTA DELLA LISTA RECENTE DEI NODI IN POSIZIONE SBAGLIATA

            while (nPR != NULL) {
                nodePos *nPS = malloc(sizeof(nodePos));

                nPS->pos = nPR->pos;
                nPS->next = pFS->pS;
                pFS->pS = nPS;

                nPR = nPR->next;
            }

            pFS->next = *listFilterSto;
            *listFilterSto = pFS;
        } else {
            // SE IL NODO ESISTE E L'ATTRIBUTO È NESSUNO LO TOLGO DA LISTFILTROREC
            // SE IL NODO ESISTE E L'ATTRIBUTO È MAGGIORE UGUALE O ESATTI DEVO VEDERE SE SONO CAMBIATI I NODI DI POSIZIONE E CONTROLLARE SE CAMBIANO ANCHE IL NUMERO DI LETTERE
            // SE IL NODO È ESATTI NON MODIFICO IN MAGGIORE UGUALE

            bool changedNumAtt = false;
            if (curr->attr > pFS->attr) {
                pFS->attr = curr->attr;
                changedNumAtt = true;
            }

            if(curr->num > pFS->num){
                pFS->num = curr->num;
                changedNumAtt = true;
            }

            nodePos *nPCurr = curr->pG;

            while (nPCurr != NULL) {
                nodePos *nPS = searchNodePos(pFS->pG, nPCurr->pos);

                if (nPS == NULL) {
                    nPS = malloc(sizeof(nodePos));
                    nPS->pos = nPCurr->pos;
                    nPS->next = pFS->pG;
                    pFS->pG = nPS;
                    nPCurr = nPCurr->next;
                }
            }

            nPCurr = pFS->pG;
            int cont = 0;

            while (nPCurr != NULL) {
                cont++;
                nPCurr = nPCurr->next;
            }

            // DEVO NOTIFICARE L'AGGIORNAMENTO MODIFICANDO LA LISTA RECENTE PER POI APPLICARLA
            if (cont > pFS->num) {
                curr->num = cont;
                changedNumAtt = true;
            }

            nPCurr = curr->pS;

            while (nPCurr != NULL) {
                nodePos *nPS = searchNodePos(pFS->pS, nPCurr->pos);

                if (nPS == NULL) {
                    nPS = malloc(sizeof(nodePos));
                    nPS->pos = nPCurr->pos;
                    nPS->next = pFS->pS;
                    pFS->pS = nPS;
                    nPCurr = nPCurr->next;
                }
            }

            if (changedNumAtt == false)
                curr->num = -1;
        }
        curr = curr->next;
    }
}

void deleteTree(nodeTreeString *treeStringValid,  nodeTreeString *NIL, nodeCamb **listCamb) {
    if(treeStringValid == NIL) return;
    insertListCamb(listCamb, treeStringValid);
    deleteTree(treeStringValid->left, NIL, listCamb);
    deleteTree(treeStringValid->right, NIL, listCamb);
}

void deleteListFilter(nodeFilter **listFilter) {
    for(nodeFilter *curF = *listFilter, *sucF = NULL; curF != NULL; curF = sucF){
        sucF = curF->next;
        nodePos *curP = curF->pG, *sucP = NULL;
        while(curP != NULL){
            sucP = curP->next;
            free(curP);
            curP = sucP;
        }

        curP = curF->pS, sucP = NULL;
        while(curP != NULL){
            sucP = curP->next;
            free(curP);
            curP = sucP;
        }

        free(curF);
    }
    *listFilter = NULL;
}

nodeLetter *searchNodeLetter(nodeLetter *list, char c){
    while(list != NULL){
        if(list->c == c)
            return list;
        list = list->next;
    }
    return NULL;
}

void deleteListLetter(nodeLetter **list){
    nodeLetter *curr = *list, *next = NULL;
    while(curr != NULL){
        next = curr->next;
        free(curr);
        curr = next;
    }
    *list = NULL;
}

void createNodeLetter(nodeLetter **list, char c){
    nodeLetter *ins = malloc(sizeof(nodeLetter));
    ins->c = c;
    ins->num = 1;
    ins->next = *list;
    *list = ins;
}

bool filtraParola(const char *string, nodeFilter *listFilter){
    int hashMap[78] = {0};

    for(int i = 0; i < k - 1; i++)
        hashMap[(int) string[i] - 45]++;

    while(listFilter != NULL){
        if(listFilter->attr == NESSUNO){
            if(hashMap[(int) listFilter->c - 45] != 0)
                return false;
        }
        else{
            if(listFilter->attr == ESATTI){
                if(hashMap[(int) listFilter->c - 45] != listFilter->num && listFilter->num != -1)
                    return false;
            }
            else{
                if(hashMap[(int) listFilter->c - 45] < listFilter->num)
                    return false;
            }

            if(controlloPosizioni(string, listFilter) == false)
                return false;

        }
        listFilter = listFilter->next;
    }
    return true;
}

void inserisciInizio(nodeTreeString **treeStringValid, nodeTreeString **treeStringAll, nodeTreeString *NIL, nodeFilter *listFilterSto, int lengthBuff) {
    char buffer[lengthBuff];

    while(1){
        if(fgets(buffer, lengthBuff, stdin) == NULL) exit(-3);
        if(stringCmp(buffer, "+inserisci_fine\n") == 0) break;
        buffer[k - 1] = '\0';
        if(listFilterSto != NULL && filtraParola(buffer, listFilterSto) == true){
            insertTreeNode(treeStringValid, NIL, createTreeNodeString(NIL, buffer));
            numeroParoleFiltrate++;
        }
        else
            insertTreeNode(treeStringAll, NIL, createTreeNodeString(NIL, buffer));

    }
}

void RBDeleteFixup(nodeTreeString **root, nodeTreeString *x, nodeTreeString *NIL){
    if(x->col == RED || x == *root)
        x->col = BLACK;
    else if(x == x->parent->left){
        nodeTreeString *w = x->parent->right;
        if(w->col == RED){
            w->col = BLACK;
            x->parent->col = RED;
            leftRotate(root, x->parent, NIL);
            w = x->parent->right;
        }

        if(w->left->col == BLACK && w->right->col == BLACK){
            w->col = RED;
            RBDeleteFixup(root, x->parent, NIL);
        }
        else{
            if(w->right->col == BLACK){
                w->left->col = BLACK;
                w->col = RED;
                rightRotate(root, w, NIL);
                w = x->parent->right;
            }
            w->col = x->parent->col;
            x->parent->col = BLACK;
            w->right->col = BLACK;
            leftRotate(root, x->parent, NIL);
        }
    }
    else{
        nodeTreeString *w = x->parent->left;
        if(w->col == RED){
            w->col = BLACK;
            x->parent->col = RED;
            rightRotate(root, x->parent, NIL);
            w = x->parent->left;
        }

        if(w->left->col == BLACK && w->right->col == BLACK){
            w->col = RED;
            RBDeleteFixup(root, x->parent, NIL);
        }
        else{
            if(w->left->col == BLACK){
                w->right->col = BLACK;
                w->col = RED;
                leftRotate(root, w, NIL);
                w = x->parent->left;
            }
            w->col = x->parent->col;
            x->parent->col = BLACK;
            w->left->col = BLACK;
            rightRotate(root, x->parent, NIL);
        }
    }
}

nodeTreeString *moveNode(nodeTreeString **source, nodeTreeString **dest, nodeTreeString *NIL, nodeTreeString *z) {
    nodeTreeString *y;
    if(z->left == NIL || z->right == NIL)
        y = z;
    else
        y = treeSuccessor(z, NIL);

    nodeTreeString *x;
    if(y->left != NIL)
        x = y->left;
    else
        x = y->right;

    x->parent = y->parent;

    if(y->parent == NIL)
        *source = x;
    else if(y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;

    color colEliminated = y->col;
    if(y != z){
        if(z->parent == NIL)
            *source = y;
        else if(z == z->parent->left)
            z->parent->left = y;
        else
            z->parent->right = y;

        y->parent = z->parent;
        y->left = z->left;
        y->left->parent = y;
        y->right = z->right;
        y->right->parent = y;
        y->col = z->col;
    }

    if(colEliminated == BLACK)
        RBDeleteFixup(source, x, NIL);

    z->parent = NIL;
    z->left = NIL;
    z->right = NIL;
    z->col = RED;

    insertTreeNode(dest, NIL, z);

    return y != z ? y : x;
}

bool controlloPosizioni(const char *string, nodeFilter *nodeFil){
    nodePos *pPos = nodeFil->pG;
    while(pPos != NULL){
        if(string[pPos->pos] != nodeFil->c)
            return false;

        pPos = pPos->next;
    }

    pPos = nodeFil->pS;
    while(pPos != NULL) {
        if (string[pPos->pos] == nodeFil->c)
            return false;

        pPos = pPos->next;
    }
    return true;
}

void insertListCamb(nodeCamb **listCamb, nodeTreeString *treeString){
    nodeCamb *ins = malloc(sizeof(nodeCamb));
    ins->nodeTreeStringCamb = treeString;
    ins->next = *listCamb;
    *listCamb = ins;
}

void filtraStringheFromAll(nodeTreeString *treeString, nodeTreeString *NIL, nodeFilter *listFilter, nodeCamb **listCamb, bool filPar){
    if(treeString == NIL) return;
    if(filtraParola(treeString->string, listFilter) == filPar)
        insertListCamb(listCamb, treeString);

    filtraStringheFromAll(treeString->left, NIL, listFilter, listCamb, filPar);
    filtraStringheFromAll(treeString->right, NIL, listFilter, listCamb, filPar);
}

void nuovaPartita(nodeTreeString **treeStringAll, nodeTreeString *NIL, int lengthBuff){
    char buffer[lengthBuff];
    nodeTreeString *treeStringValid = NIL;
    nodeCamb *listCamb = NULL;
    nodeFilter *listFilterRec = NULL;
    nodeFilter *listFilterSto = NULL;
    int n;

    if(fgets(buffer, lengthBuff, stdin) == NULL) exit(-3);
    buffer[k - 1] = '\0';

    nodeTreeString *pR = searchBST(*treeStringAll, NIL, buffer);

    if(fgets(buffer, lengthBuff, stdin) == NULL) exit(-3);
    n = (int) strtol(buffer, NULL, 10);

    while(n > 0){
        if(fgets(buffer, lengthBuff, stdin) == NULL) exit(-3);

        if(stringCmp(buffer, "+stampa_filtrate\n") == 0){
            if(treeStringValid != NIL)
                inOrderTraverseTreeValid(treeStringValid, NIL);
            else
                inOrderTraverseTreeValid(*treeStringAll, NIL);
            continue;
        }

        if(stringCmp(buffer, "+inserisci_inizio\n") == 0){
            inserisciInizio(&treeStringValid, treeStringAll, NIL, listFilterSto, lengthBuff);
            continue;
        }
        buffer[k - 1] = '\0';
        if(stringCmp(buffer, pR->string) == 0){
            puts("ok");
            break;
        }

        if(searchBST(*treeStringAll, NIL, buffer) != NIL || searchBST(treeStringValid, NIL, buffer) != NIL){
            n--;
            for(int i = 0; i < k - 1; i++){
                if(buffer[i] == pR->string[i]){
                    putchar('+');
                    filtroLettera(listFilterSto, &listFilterRec, buffer[i], i, 2);
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
                        putchar('/');
                        filtroLettera(listFilterSto, &listFilterRec, buffer[i], 0, 1);
                    }
                    else{
                        int lettereNonInPosizioneCorretta = 0;
                        for(int j = 0; j <= i; j++){
                            if(buffer[i] == buffer[j] && buffer[j] != pR->string[j])
                                lettereNonInPosizioneCorretta++;
                        }

                        if(lettereNonInPosizioneCorretta <= lettereLibere){
                            putchar('|');
                            filtroLettera(listFilterSto, &listFilterRec, buffer[i], i, 3);
                        }
                        else{
                            putchar('/');
                            filtroLettera(listFilterSto, &listFilterRec, buffer[i], i, 4);
                        }
                    }
                }
            }
            putchar('\n');
            // Applicare il filtro recente a quello storico
            // Vedere se ci sono delle modifiche in quello storico rispetto al recente (numero di lettere)
            // Se ci sono delle modifiche, aggiorno la lista recente e poi la applico
            aggiornamentoFiltroStorico(&listFilterRec, &listFilterSto);
            // Filtrare le stringhe
            if(treeStringValid == NIL){
                filtraStringheFromAll(*treeStringAll, NIL, listFilterRec, &listCamb, true);
                modifyValidTree(treeStringAll, &treeStringValid, NIL, &listCamb, true);
            }
            else{
                filtraStringheFromAll(treeStringValid, NIL, listFilterRec, &listCamb, false);
                modifyValidTree(treeStringAll, &treeStringValid, NIL, &listCamb, false);
            }

            printf("%d\n", numeroParoleFiltrate);
            deleteListFilter(&listFilterRec);
        }
        else
            puts("not_exists");

        if(n == 0)
            puts("ko");
    }

    deleteTree(treeStringValid, NIL, &listCamb);

    nodeCamb *curr = listCamb, *next = NULL;
    while(curr != NULL){
        next = curr->next;
        moveNode(&treeStringValid, treeStringAll, NIL, curr->nodeTreeStringCamb);
        free(curr);
        curr = next;
    }

    deleteListFilter(&listFilterSto);
}

void modifyValidTree(nodeTreeString **treeStringAll, nodeTreeString **treeStringValid, nodeTreeString *NIL, nodeCamb **listCamb, bool allToValid) {
    nodeCamb *curr = *listCamb, *next = NULL;

    while(curr != NULL){
        next = curr->next;
        if(allToValid == true){
            moveNode(treeStringAll, treeStringValid, NIL, curr->nodeTreeStringCamb);
            numeroParoleFiltrate++;
        }
        else{
            moveNode(treeStringValid, treeStringAll, NIL, curr->nodeTreeStringCamb);
            numeroParoleFiltrate--;
        }
        free(curr);
        curr = next;
    }
    *listCamb = NULL;
}

int main() {
    nodeTreeString *NIL = createNIL();
    nodeTreeString *treeStringValid = NIL;
    int lengthBuff;
    char input[10];

    if(fgets(input, 10, stdin) == NULL) exit(-1);

    k = (int) strtol(input, NULL, 10);
    k++;
    if(k > 18)
        lengthBuff = k + 1;
    else
        lengthBuff = 19;

    creazioneParole(&treeStringValid, NIL, lengthBuff, "+nuova_partita\n");
    nuovaPartita(&treeStringValid, NIL, lengthBuff);

    char buffer[lengthBuff];

    do{
        if(fgets(buffer, lengthBuff, stdin) == NULL) break;

        if(stringCmp(buffer, "+inserisci_inizio\n") == 0)
            creazioneParole(&treeStringValid, NIL, lengthBuff, "+inserisci_fine\n");
        else if(stringCmp(buffer, "+nuova_partita\n") == 0){
            numeroParoleFiltrate = 0;
            nuovaPartita(&treeStringValid, NIL, lengthBuff);
        }
    } while (1);

    return 0;
}