#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum{NESSUNO, MAGGIOREUGUALE, ESATTI} attributoLettere;
typedef enum {RED, BLACK} color;

const int numberInput = 100;
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

void createTreeNodeString(nodeTreeString* parent, nodeTreeString* node, char *string, color col){
    node->string = malloc(sizeof(char) * k);
    strcpy(node->string, string);
    node->left = NULL;
    node->right = NULL;
    node->parent = parent;
    node->col = col;
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

void insertTree(nodeTreeString** head, nodeTreeString* z, char *string){
    nodeTreeString* y = NULL;
    nodeTreeString* x = *head;

    while(x != NULL){
        y = x;
        if(strcmp(string, x->string) < 0)
            x = x->left;
        else
            x = x->right;
    }

    createTreeNodeString(y, z, string, RED);

    if(y == NULL)
        *head = z;
    else if (strcmp(z->string, y->string) < 0)
        y->left = z;
    else
        y->right = z;

    RBInsertFixup(head, z);
}

nodeTreeString* searchBST(nodeTreeString* head, char* string){
    while(head != NULL){
        int n = strcmp(string, head->string);
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
    int i = 0;
    char buffer[numberInput][lengthBuff];
    nodeTreeString *z;

    while(1){
        if(fgets(buffer[i], lengthBuff, stdin) == NULL) exit(-2);
        if(strcmp(buffer[i], endString) == 0) break;
        buffer[i][k - 1] = '\0';
        i++;
        if(i == numberInput){
            z = malloc(sizeof(nodeTreeString) * i);
            for(int j = 0; j < i; j++)
                insertTree(treeStringValid, &z[j], buffer[j]);
            i = 0;
        }
    }

    if(i != 0){
        z = malloc(sizeof(nodeTreeString) * i);
        for(int j = 0; j < i; j++)
            insertTree(treeStringValid, &z[j], buffer[j]);
    }

    i = 0;
}

void deleteAllTree(nodeTreeString *root) {
    if(root == NULL) return;
    deleteAllTree(root->left);
    deleteAllTree(root->right);
    free(root->string);
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
        if(strcmp(ins->string, x->string) < 0)
            x = x->left;
        else
            x = x->right;
    }

    ins->parent = y;

    if(y == NULL)
        *treeStringInvalid = ins;
    else if (strcmp(ins->string, y->string) < 0)
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

void filtraNessuno(nodeTreeString **treeStringValid, nodeTreeString **treeStringInvalid, nodeTreeString *nodeTr, char c){
    if(nodeTr == NULL) return;
    bool delete;
    do {
        delete = false;
        for(int i = 0; i < k - 1 && delete == false; i++){
            if(nodeTr->string[i] == c)
                delete = true;
        }

        if(delete == true){
            nodeTr = moveNode(treeStringValid, treeStringInvalid, nodeTr);
        }
        else
            break;
    }while(nodeTr != NULL);

    if(nodeTr != NULL){
        filtraNessuno(treeStringValid, treeStringInvalid, nodeTr->left, c);
        filtraNessuno(treeStringValid, treeStringInvalid, nodeTr->right, c);
    }
}

void filtraMaggioreUguale(nodeTreeString **treeStringValid, nodeTreeString **treeStringInvalid, nodeTreeString *nodeTr, nodeFilter *nodeFilt){
    if(nodeTr == NULL) return;
    int cont, min;
    do {
        cont = 0;
        min = nodeFilt->num;
        for(int i = 0; i < k - 1 && cont < min; i++){
            if(nodeTr->string[i] == nodeFilt->c)
                cont++;
        }

        if(cont < min || controlloPosizioni(nodeTr, nodeFilt) == true)
            nodeTr = moveNode(treeStringValid, treeStringInvalid, nodeTr);
        else
            break;

    }while(nodeTr != NULL);

    if(nodeTr != NULL){
        filtraMaggioreUguale(treeStringValid, treeStringInvalid,  nodeTr->left, nodeFilt);
        filtraMaggioreUguale(treeStringValid, treeStringInvalid,  nodeTr->right, nodeFilt);
    }
}

void filtraEsatti(nodeTreeString **treeStringValid, nodeTreeString **treeStringInvalid, nodeTreeString *nodeTr, nodeFilter *nodeFilt){
    if(nodeTr == NULL) return;
    int cont, esatti;
    do {
        cont = 0;
        esatti = nodeFilt->num;
        for(int i = 0; i < k - 1; i++){
            if(nodeTr->string[i] == nodeFilt->c)
                cont++;
        }

        if(cont != esatti || controlloPosizioni(nodeTr, nodeFilt) == true)
            nodeTr = moveNode(treeStringValid, treeStringInvalid, nodeTr);
        else
            break;
    }while(nodeTr != NULL);

    if(nodeTr != NULL){
        filtraEsatti(treeStringValid, treeStringInvalid, nodeTr->left, nodeFilt);
        filtraEsatti(treeStringValid, treeStringInvalid, nodeTr->right, nodeFilt);
    }
}

void filtraStringhe(nodeTreeString **treeStringValid, nodeTreeString **treeStringInvalid, nodeFilter *listFilter) {
    while(listFilter != NULL){
        if(listFilter->attr == NESSUNO)
            filtraNessuno(treeStringValid, treeStringInvalid, *treeStringValid, listFilter->c);
        else if(listFilter->attr == MAGGIOREUGUALE)
            filtraMaggioreUguale(treeStringValid, treeStringInvalid, *treeStringValid, listFilter);
        else
            filtraEsatti(treeStringValid, treeStringInvalid, *treeStringValid, listFilter);
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
        if(strcmp(buffer, "+inserisci_fine\n") == 0) break;
        buffer[k - 1] = '\0';
        nodeTreeString *z = malloc(sizeof(nodeTreeString));
        if(filtraParola(buffer, listFiltroSto) == true)
            insertTree(treeStringValid, z, buffer);
        else
            insertTree(treeStringInvalid, z, buffer);
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
    nodeFilter *listFiltroRec = NULL;
    nodeFilter *listFiltroSto = NULL;
    int n;

    if(fgets(buffer, lengthBuff, stdin) == NULL) exit(-3);
    buffer[k - 1] = '\0';

    nodeTreeString *pR = searchBST(*treeStringValid, buffer);

    if(fgets(buffer, lengthBuff, stdin) == NULL) exit(-3);
    n = (int) strtol(buffer, NULL, 10);

    while(n > 0){
        if(fgets(buffer, lengthBuff, stdin) == NULL) exit(-3);

        if(strcmp(buffer, "+stampa_filtrate\n") == 0){
            inOrderTraverseTreeValid(*treeStringValid);
            continue;
        }

        if(strcmp(buffer, "+inserisci_inizio\n") == 0){
            inserisciInizio(treeStringValid, &treeStringInvalid, listFiltroSto, lengthBuff);
            continue;
        }

        buffer[k - 1] = '\0';

        if(strcmp(buffer, pR->string) == 0){
            printf("ok\n");
            break;
        }

        if(searchBST(treeStringInvalid, buffer) != NULL || searchBST(*treeStringValid, buffer) != NULL){
            n--;
            for(int i = 0; i < k - 1; i++){
                if(buffer[i] == pR->string[i]){
                    putchar('+');
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
                        putchar('/');
                        letteraInesistente(&listFiltroRec, buffer[i]);
                    }
                    else{
                        int lettereNonInPosizioneCorretta = 0;
                        for(int j = 0; j <= i; j++){
                            if(buffer[i] == buffer[j] && buffer[j] != pR->string[j])
                                lettereNonInPosizioneCorretta++;
                        }

                        if(lettereNonInPosizioneCorretta <= lettereLibere){
                            putchar('|');
                            letteraPosizioneSbagliata(&listFiltroRec, buffer[i], i);
                        }
                        else{
                            putchar('/');
                            letteraConnessioniFinite(&listFiltroRec, buffer[i], i);
                        }
                    }
                }
            }
            putchar('\n');
            // Applicare il filtro recente a quello storico
            // Vedere se ci sono delle modifiche in quello storico rispetto al recente (numero di lettere)
            // Se ci sono delle modifiche, aggiorno la lista recente e poi la applico
            aggiornamentoFiltroStorico(listFiltroRec, &listFiltroSto);
            // Filtrare le stringhe
            filtraStringhe(treeStringValid, &treeStringInvalid, listFiltroRec);
            printf("%d\n", numeroStringheValide(*treeStringValid));
            freeListaFiltro(&listFiltroRec);
        }
        else
            puts("not_exists");

        if(n == 0)
            puts("ko");
    }

    deleteTreeStringValid(*treeStringValid, &treeStringInvalid);
    *treeStringValid = treeStringInvalid;
    freeListaFiltro(&listFiltroSto);
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

        if(strcmp(buffer, "+inserisci_inizio\n") == 0)
            creazioneParole(&treeStringValid, lengthBuff, "+inserisci_fine\n");
        else if(strcmp(buffer, "+nuova_partita\n") == 0)
            nuovaPartita(&treeStringValid, lengthBuff);

    } while (1);

    /*deleteAllTree(treeStringValid);
    free(treeStringValid);*/
    return 0;
}