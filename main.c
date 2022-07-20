#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum{NESSUNO, MAGGIOREUGUALE, ESATTI} attributoLettere;
typedef enum {RED, BLACK} color;

int k;

typedef struct treeString{
    char *string;
    struct treeString* left;
    struct treeString* right;
    struct treeString* parent;
    color col;
}nodeTreeString;

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

void setLengthBuff(int *lengthBuff){
    if(k > 18)
        *lengthBuff = k + 1;
    else
        *lengthBuff = 19;
}

int stringCmp(const char* s1, const char* s2){
    while (*s1 && (*s1 == *s2))
        s1++, s2++;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

nodeTreeString* createTreeNodeString(nodeTreeString* parent, char *string, color col){
    nodeTreeString* result = malloc(sizeof(nodeTreeString));
    if(result != NULL){
        result->string = malloc(sizeof(char) * k);
        strcpy(result->string, string);
        result->left = NULL;
        result->right = NULL;
        result->parent = parent;
        result->col = col;
    }
    return result;
}

void leftRotate(nodeTreeString **root, nodeTreeString* x){
    nodeTreeString *y = x->right;
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

void rightRotate(nodeTreeString **root, nodeTreeString* x){
    nodeTreeString *y = x->left;
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

void RBInsertFixup(nodeTreeString **root, nodeTreeString *z) {
    if(z == *root)
        (*root)->col = BLACK;
    else{
        nodeTreeString *x = z->parent; // x è il padre di z
        if(x->col == RED){
            if(x == x->parent->left){
                nodeTreeString *y = x->parent->right; // y è lo zio di z (fratello di x)

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
                nodeTreeString *y = x->parent->left; // y è lo zio di z (fratello di x)

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

void insertTree(nodeTreeString** head, char *string){
    nodeTreeString* y = NULL;
    nodeTreeString* x = *head;

    while(x != NULL){
        y = x;
        if(stringCmp(string, x->string) < 0)
            x = x->left;
        else
            x = x->right;
    }

    nodeTreeString* z = createTreeNodeString(y, string, RED);

    if(y == NULL)
        *head = z;
    else if (stringCmp(z->string, y->string) < 0)
        y->left = z;
    else
        y->right = z;

    RBInsertFixup(head, z);
}

nodeTreeString* searchBST(nodeTreeString* head, char* string){
    while(head != NULL){
        int n = stringCmp(string, head->string);
        if(n == 0)
            return head;
        else if(n < 0)
            head = head->left;
        else
            head = head->right;
    }
    return head;
}

nodeTreeString* treeMinimum(nodeTreeString* head){
    while(head->left != NULL)
        head = head->left;
    return head;
}

nodeTreeString* treeSuccessor(nodeTreeString* treeString, nodeTreeString* x){
    if(x->right != NULL)
        return treeMinimum(x->right);

    nodeTreeString* y = x->parent;
    while(y != NULL && x == y->right){
        x = y;
        y = y->parent;
    }
    return y;
}

void creazioneParole(nodeTreeString **treeStringValid, int lengthBuff, char *endString){
    char buffer[lengthBuff];

    while(1){
        if(fgets(buffer, lengthBuff, stdin) == NULL) exit(-2);
        if(stringCmp(buffer, endString) == 0) break;
        buffer[k - 1] = '\0';
        insertTree(treeStringValid, buffer);
    }
}

void deleteAllTree(nodeTreeString *root) {
    if(root == NULL) return;
    deleteAllTree(root->left);
    deleteAllTree(root->right);
    free(root->string);
    free(root);
}

nodePos* searchNodePos(nodePos *listNodePos, int pos) {
    while(listNodePos != NULL){
        if(listNodePos->pos == pos)
            return listNodePos;
        listNodePos = listNodePos->pn;
    }
    return NULL;
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

void letteraGiusta(nodeFilter *listFilterSto, nodeFilter **listFilterRec, char c, int pos) {
    nodeFilter *puntatoreNodoFiltroSto = searchListFilter(listFilterSto, c);
    nodeFilter *puntatoreNodoFiltroRec = searchListFilter(*listFilterRec, c);

    if(puntatoreNodoFiltroRec == NULL){
        puntatoreNodoFiltroRec = malloc(sizeof(nodeFilter));
        puntatoreNodoFiltroRec->c = c;
        puntatoreNodoFiltroRec->num = 1;
        puntatoreNodoFiltroRec->attr = MAGGIOREUGUALE;
        puntatoreNodoFiltroRec->pS = NULL;

        if(puntatoreNodoFiltroSto == NULL || (puntatoreNodoFiltroSto != NULL && searchNodePos(puntatoreNodoFiltroSto->pG, pos) == NULL)){
            nodePos *pPos = malloc(sizeof(nodePos));
            pPos->pos = pos;
            puntatoreNodoFiltroRec->pG = pPos;
            pPos->pn = NULL;
        }
        else
            puntatoreNodoFiltroRec->pG = NULL;

        puntatoreNodoFiltroRec->pn = *listFilterRec;
        *listFilterRec = puntatoreNodoFiltroRec;
    }
    else{
        puntatoreNodoFiltroRec->num++;
        if(puntatoreNodoFiltroSto == NULL || (puntatoreNodoFiltroSto != NULL && searchNodePos(puntatoreNodoFiltroSto->pG, pos) == NULL)){
            nodePos *pPos = malloc(sizeof(nodePos));
            pPos->pos = pos;
            pPos->pn = puntatoreNodoFiltroRec->pG;
            puntatoreNodoFiltroRec->pG = pPos;
        }
    }
}

void letteraInesistente(nodeFilter *listFilterSto, nodeFilter **listFilterRec, char c) {
    nodeFilter *pF = searchListFilter(listFilterSto, c);

    if(pF == NULL && searchListFilter(*listFilterRec, c) == NULL){
        pF = malloc(sizeof(nodeFilter));
        pF->c = c;
        pF->num = 0;
        pF->attr = NESSUNO;
        pF->pG = NULL;
        pF->pS = NULL;

        pF->pn = *listFilterRec;
        *listFilterRec = pF;
    }
}

void letteraPosizioneSbagliata(nodeFilter *listFilterSto, nodeFilter **listFilterRec, char c, int pos) {
    nodeFilter *puntatoreNodoFiltroSto = searchListFilter(listFilterSto, c);
    nodeFilter *puntatoreNodoFiltroRec = searchListFilter(*listFilterRec, c);

    if(puntatoreNodoFiltroRec == NULL){
        puntatoreNodoFiltroRec = malloc(sizeof(nodeFilter));
        puntatoreNodoFiltroRec->c = c;
        puntatoreNodoFiltroRec->num = 1;
        puntatoreNodoFiltroRec->attr = MAGGIOREUGUALE;
        puntatoreNodoFiltroRec->pG = NULL;

        if(puntatoreNodoFiltroSto == NULL || (puntatoreNodoFiltroSto != NULL && searchNodePos(puntatoreNodoFiltroSto->pS, pos) == NULL)){
            nodePos *pPos = malloc(sizeof(nodePos));
            pPos->pos = pos;
            puntatoreNodoFiltroRec->pS = pPos;
            pPos->pn = NULL;
        }
        else
            puntatoreNodoFiltroRec->pS = NULL;

        puntatoreNodoFiltroRec->pn = *listFilterRec;
        *listFilterRec = puntatoreNodoFiltroRec;
    }
    else{
        puntatoreNodoFiltroRec->num++;
        if(puntatoreNodoFiltroSto == NULL || (puntatoreNodoFiltroSto != NULL && searchNodePos(puntatoreNodoFiltroSto->pS, pos) == NULL)){
            nodePos *pPos = malloc(sizeof(nodePos));
            pPos->pos = pos;
            pPos->pn = puntatoreNodoFiltroRec->pS;
            puntatoreNodoFiltroRec->pS = pPos;
        }
    }
}

void letteraConnessioniFinite(nodeFilter *listFilterSto, nodeFilter **listFilterRec, char c, int pos) {
    nodeFilter *puntatoreNodoFiltroSto = searchListFilter(listFilterSto, c);
    nodeFilter *puntatoreNodoFiltroRec = searchListFilter(*listFilterRec, c);

    if(puntatoreNodoFiltroRec == NULL){
        puntatoreNodoFiltroRec = malloc(sizeof(nodeFilter));
        puntatoreNodoFiltroRec->c = c;
        puntatoreNodoFiltroRec->num = 0;
        puntatoreNodoFiltroRec->attr = ESATTI;
        puntatoreNodoFiltroRec->pG = NULL;

        if(puntatoreNodoFiltroSto == NULL || (puntatoreNodoFiltroSto != NULL && searchNodePos(puntatoreNodoFiltroSto->pS, pos) == NULL)){
            nodePos *pPos = malloc(sizeof(nodePos));
            pPos->pos = pos;
            puntatoreNodoFiltroRec->pS = pPos;
            pPos->pn = NULL;
        }
        else
            puntatoreNodoFiltroRec->pS = NULL;

        puntatoreNodoFiltroRec->pn = *listFilterRec;
        *listFilterRec = puntatoreNodoFiltroRec;
    }
    else{
        puntatoreNodoFiltroRec->attr = ESATTI;
        if(puntatoreNodoFiltroSto == NULL || (puntatoreNodoFiltroSto != NULL && searchNodePos(puntatoreNodoFiltroSto->pS, pos) == NULL)){
            nodePos *pPos = malloc(sizeof(nodePos));
            pPos->pos = pos;
            pPos->pn = puntatoreNodoFiltroRec->pS;
            puntatoreNodoFiltroRec->pS = pPos;
        }
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

void inOrderTraverseTreeValid(nodeTreeString *treeStringValid){
    if(treeStringValid == NULL) return;
    inOrderTraverseTreeValid(treeStringValid->left);
    printf("%s\n", treeStringValid->string);
    inOrderTraverseTreeValid(treeStringValid->right);
}

int numeroStringheValide(nodeTreeString *treeStringValid) {
    if(treeStringValid == NULL) return 0;
    return 1 + numeroStringheValide(treeStringValid->left) + numeroStringheValide(treeStringValid->right);
}

bool controlloPosizioni(nodeTreeString *curr, nodeFilter *nodeFil){
    bool delete = false;
    nodePos *pPos = nodeFil->pG;

    while(pPos != NULL && delete == false){
        if(curr->string[pPos->pos] != nodeFil->c)
            delete = true;

        pPos = pPos->pn;
    }

    pPos = nodeFil->pS;

    while(pPos != NULL && delete == false) {
        if (curr->string[pPos->pos] == nodeFil->c)
            delete = true;

        pPos = pPos->pn;
    }

    return delete;
}

void insertNodeTreeInvalid(nodeTreeString **treeStringInvalid, nodeTreeString *ins){
    nodeTreeString* y = NULL;
    nodeTreeString* x = *treeStringInvalid;

    while(x != NULL){
        y = x;
        if(stringCmp(ins->string, x->string) < 0)
            x = x->left;
        else
            x = x->right;
    }

    ins->parent = y;

    if(y == NULL)
        *treeStringInvalid = ins;
    else if (stringCmp(ins->string, y->string) < 0)
        y->left = ins;
    else
        y->right = ins;

    RBInsertFixup(treeStringInvalid, ins);
}

nodeTreeString *moveNode(nodeTreeString **treeStringValid, nodeTreeString **treeStringInvalid, nodeTreeString *del){
    nodeTreeString *y = NULL, *x = NULL;

    if(del->left == NULL || del->right == NULL)
        y = del;
    else
        y = treeSuccessor(*treeStringValid, del);

    if(y->left != NULL)
        x = y->left;
    else
        x = y->right;

    if(x != NULL)
        x->parent = y->parent;

    if(y->parent == NULL)
        *treeStringValid = x;
    else if(y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;

    if(y != del){
        y->parent = del->parent;
        y->left = del->left;
        y->right = del->right;
        if(del->parent != NULL){
            if(del == del->parent->left)
                del->parent->left = y;
            else
                del->parent->right = y;
        }
        else
            *treeStringValid = y;

        if(del->left != NULL)
            del->left->parent = y;

        if(del->right != NULL)
            del->right->parent = y;

        del->parent = NULL;
        del->left = NULL;
        del->right = NULL;
        del->col = RED;
        insertNodeTreeInvalid(treeStringInvalid, del);
        return y;
    }

    del->parent = NULL;
    del->left = NULL;
    del->right = NULL;
    del->col = RED;
    insertNodeTreeInvalid(treeStringInvalid, del);
    return x;
}

void filtraStringhe(nodeTreeString **treeStringValid, nodeTreeString **treeStringInvalid, nodeTreeString *nodeTr, nodeFilter *listFilter){
    if(nodeTr == NULL) return;

    nodeFilter *headListFilter = listFilter;

    while(nodeTr != NULL){
        int hashMap[78] = {0};
        for(int i = 0; i < k; i++)
            hashMap[(int) nodeTr->string[i] - 45]++;
        while(listFilter != NULL){
            if(listFilter->attr == NESSUNO){
                if(hashMap[(int) listFilter->c - 45] != 0){
                    nodeTr = moveNode(treeStringValid, treeStringInvalid, nodeTr);
                    break;
                }
            }
            else if(listFilter->attr == MAGGIOREUGUALE && (listFilter->pG != NULL || listFilter->pS != NULL || listFilter->num != -1)){
                if(controlloPosizioni(nodeTr, listFilter) == true){
                    nodeTr = moveNode(treeStringValid, treeStringInvalid, nodeTr);
                    break;
                }
                else if(listFilter->num != -1){
                    int min = listFilter->num;
                    if(hashMap[(int) listFilter->c - 45] < min){
                        nodeTr = moveNode(treeStringValid, treeStringInvalid, nodeTr);
                        break;
                    }
                }
            }
            else if(listFilter->attr == ESATTI && (listFilter->pG != NULL || listFilter->pS != NULL || listFilter->num != -1)){
                if(controlloPosizioni(nodeTr, listFilter) == true){
                    nodeTr = moveNode(treeStringValid, treeStringInvalid, nodeTr);
                    break;
                }
                else if(listFilter->num != -1){
                    int esatti = listFilter->num;
                    if(hashMap[(int) listFilter->c - 45] != esatti){
                        nodeTr = moveNode(treeStringValid, treeStringInvalid, nodeTr);
                        break;
                    }
                }
            }
            listFilter = listFilter->pn;
        }
        if(listFilter == NULL)
            break;
        listFilter = headListFilter;
    }

    if(nodeTr != NULL){
        filtraStringhe(treeStringValid, treeStringInvalid, nodeTr->left, headListFilter);
        filtraStringhe(treeStringValid, treeStringInvalid, nodeTr->right, headListFilter);
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

void aggiornamentoFiltroStorico(nodeFilter **listFiltroRec, nodeFilter **listFiltroSto) {
    nodeFilter *curr = *listFiltroRec;
    while(curr != NULL) {
        nodeFilter *pFS = searchListFiltroSto(*listFiltroSto, curr->c);

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
                nPS->pn = pFS->pG;
                pFS->pG = nPS;

                nPR = nPR->pn;
            }

            nPR = curr->pS; // TESTA DELLA LISTA RECENTE DEI NODI IN POSIZIONE SBAGLIATA

            while (nPR != NULL) {
                nodePos *nPS = malloc(sizeof(nodePos));

                nPS->pos = nPR->pos;
                nPS->pn = pFS->pS;
                pFS->pS = nPS;

                nPR = nPR->pn;
            }

            pFS->pn = *listFiltroSto;
            *listFiltroSto = pFS;
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
                    nPS->pn = pFS->pG;
                    pFS->pG = nPS;
                    nPCurr = nPCurr->pn;
                }
            }

            nPCurr = pFS->pG;
            int cont = 0;

            while (nPCurr != NULL) {
                cont++;
                nPCurr = nPCurr->pn;
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
                    nPS->pn = pFS->pS;
                    pFS->pS = nPS;
                    nPCurr = nPCurr->pn;
                }
            }

            if (changedNumAtt == false)
                curr->num = -1;
        }

        curr = curr->pn;
    }
}

bool filtraParola(char *string, nodeFilter *listFiltroSto){
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

void inserisciInizio(nodeTreeString **treeStringValid, nodeTreeString **treeStringInvalid, nodeFilter *listFiltroSto, int lengthBuff) {
    char buffer[lengthBuff];

    while(1){
        if(fgets(buffer, lengthBuff, stdin) == NULL) exit(-3);
        if(stringCmp(buffer, "+inserisci_fine\n") == 0) break;
        buffer[k - 1] = '\0';
        if(filtraParola(buffer, listFiltroSto) == true)
            insertTree(treeStringValid, buffer);
        else
            insertTree(treeStringInvalid, buffer);
    }

}

void deleteTreeStringValid(nodeTreeString *treeStringValid, nodeTreeString **treeStringInvalid) {
    if(treeStringValid == NULL) return;
    deleteTreeStringValid(treeStringValid->left, treeStringInvalid);
    deleteTreeStringValid(treeStringValid->right, treeStringInvalid);
    treeStringValid->parent = NULL;
    treeStringValid->left = NULL;
    treeStringValid->right = NULL;
    treeStringValid->col = RED;
    insertNodeTreeInvalid(treeStringInvalid, treeStringValid);
}

void nuovaPartita(nodeTreeString **treeStringValid, int lengthBuff){
    char buffer[lengthBuff];
    nodeTreeString *treeStringInvalid = NULL;
    nodeFilter *listFilterRec = NULL;
    nodeFilter *listFilterSto = NULL;
    int n;

    if(fgets(buffer, lengthBuff, stdin) == NULL) exit(-3);
    buffer[k - 1] = '\0';

    nodeTreeString *pR = searchBST(*treeStringValid, buffer);

    if(fgets(buffer, lengthBuff, stdin) == NULL) exit(-3);
    n = (int) strtol(buffer, NULL, 10);

    while(n > 0){
        if(fgets(buffer, lengthBuff, stdin) == NULL) exit(-3);

        if(stringCmp(buffer, "+stampa_filtrate\n") == 0){
            inOrderTraverseTreeValid(*treeStringValid);
            continue;
        }

        if(stringCmp(buffer, "+inserisci_inizio\n") == 0){
            inserisciInizio(treeStringValid, &treeStringInvalid, listFilterSto, lengthBuff);
            continue;
        }

        buffer[k - 1] = '\0';

        if(stringCmp(buffer, pR->string) == 0){
            puts("ok");
            break;
        }

        if(searchBST(treeStringInvalid, buffer) != NULL || searchBST(*treeStringValid, buffer) != NULL){
            n--;
            for(int i = 0; i < k - 1; i++){
                if(buffer[i] == pR->string[i]){
                    putchar('+');
                    letteraGiusta(listFilterSto, &listFilterRec, buffer[i], i);
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
                        letteraInesistente(listFilterSto, &listFilterRec, buffer[i]);
                    }
                    else{
                        int lettereNonInPosizioneCorretta = 0;
                        for(int j = 0; j <= i; j++){
                            if(buffer[i] == buffer[j] && buffer[j] != pR->string[j])
                                lettereNonInPosizioneCorretta++;
                        }

                        if(lettereNonInPosizioneCorretta <= lettereLibere){
                            putchar('|');
                            letteraPosizioneSbagliata(listFilterSto, &listFilterRec, buffer[i], i);
                        }
                        else{
                            putchar('/');
                            letteraConnessioniFinite(listFilterSto, &listFilterRec, buffer[i], i);
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
            filtraStringhe(treeStringValid, &treeStringInvalid, *treeStringValid, listFilterRec);
            printf("%d\n", numeroStringheValide(*treeStringValid));
            freeListaFiltro(&listFilterRec);
        }
        else
            puts("not_exists");

        if(n == 0)
            puts("ko");
    }

    deleteTreeStringValid(*treeStringValid, &treeStringInvalid);
    *treeStringValid = treeStringInvalid;
    freeListaFiltro(&listFilterSto);
}

int main() {
    int lengthBuff;
    nodeTreeString* treeStringValid = NULL;
    char input[10];

    if(fgets(input, 10, stdin) == NULL) exit(-1);

    k = (int) strtol(input, NULL, 10);
    k++;
    setLengthBuff(&lengthBuff);

    creazioneParole(&treeStringValid, lengthBuff, "+nuova_partita\n");
    nuovaPartita(&treeStringValid, lengthBuff);

    char buffer[lengthBuff];

    do{
        if(fgets(buffer, lengthBuff, stdin) == NULL) break;

        if(stringCmp(buffer, "+inserisci_inizio\n") == 0)
            creazioneParole(&treeStringValid, lengthBuff, "+inserisci_fine\n");
        else if(stringCmp(buffer, "+nuova_partita\n") == 0)
            nuovaPartita(&treeStringValid, lengthBuff);

    } while (1);

    //deleteAllTree(treeStringValid);
    return 0;
}