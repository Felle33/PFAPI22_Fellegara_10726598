#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum{NESSUNO, MAGGIOREUGUALE, ESATTI} attributoLettere;
typedef enum {RED, BLACK} color;

typedef struct tree{
    char *string;
    struct tree* left;
    struct tree* right;
    struct tree* parent;
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
        result->col = col;
    }
    return result;
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

nodeTree *deleteTreeNode(nodeTree **head, nodeTree *del) {
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

    if(y != z){
        strcpy(z->string, y->string);
        free(y->string);
        free(y);
        return z;
    }
    free(y->string);
    free(y);
    return x;
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

nodeTree *createTreeValidRec(nodeTree *treeString, nodeTree *parent, int k){
    if(treeString == NULL)
        return NULL;
    nodeTree *newNode = createTreeNode(parent, treeString->string, k, treeString->col);
    newNode->left = createTreeValidRec(treeString->left, newNode, k);
    newNode->right = createTreeValidRec(treeString->right, newNode, k);
    return newNode;
}

void inOrderTraverseTreeValid(nodeTree* treeString){
    if(treeString == NULL) return;
    inOrderTraverseTreeValid(treeString->left);
    printf("%s\n", treeString->string);
    inOrderTraverseTreeValid(treeString->right);
}

int numeroStringheValide(nodeTree *treeStringValid) {
    if(treeStringValid == NULL) return 0;
    return 1 + numeroStringheValide(treeStringValid->left) + numeroStringheValide(treeStringValid->right);
}

bool controlloPosizioni(nodeTree *curr, nodeFilter *nodeFil){
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

void filtraNessuno(nodeTree **treeStringValid, nodeTree *nodeTr, char c, int k){
    if(nodeTr == NULL) return;
    bool delete;
    do {
        delete = false;
        for(int i = 0; i < k - 1 && delete == false; i++){
            if(nodeTr->string[i] == c)
                delete = true;
        }

        if(delete == true)
            nodeTr = deleteTreeNode(treeStringValid, nodeTr);
        else
            break;
    }while(nodeTr != NULL);

    if(nodeTr != NULL){
        filtraNessuno(treeStringValid, nodeTr->left, c, k);
        filtraNessuno(treeStringValid, nodeTr->right, c, k);
    }
}

void filtraMaggioreUguale(nodeTree **treeStringValid, nodeTree *nodeTr, nodeFilter *nodeFilt, int k){
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
            nodeTr = deleteTreeNode(treeStringValid, nodeTr);
        else
            break;

    }while(nodeTr != NULL);

    if(nodeTr != NULL){
        filtraMaggioreUguale(treeStringValid, nodeTr->left, nodeFilt, k);
        filtraMaggioreUguale(treeStringValid, nodeTr->right, nodeFilt, k);
    }
}

void filtraEsatti(nodeTree **treeStringValid, nodeTree *nodeTr, nodeFilter *nodeFilt, int k){
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
            nodeTr = deleteTreeNode(treeStringValid, nodeTr);
        else
            break;
    }while(nodeTr != NULL);

    if(nodeTr != NULL){
        filtraEsatti(treeStringValid, nodeTr->left, nodeFilt, k);
        filtraEsatti(treeStringValid, nodeTr->right, nodeFilt, k);
    }
}

void filtraStringhe(nodeTree **treeStringValid, nodeFilter *listFilter, int k) {
    while(listFilter != NULL){
        if(listFilter->attr == NESSUNO)
            filtraNessuno(treeStringValid, *treeStringValid, listFilter->c, k);
        else if(listFilter->attr == MAGGIOREUGUALE)
            filtraMaggioreUguale(treeStringValid, *treeStringValid, listFilter, k);
        else
            filtraEsatti(treeStringValid, *treeStringValid, listFilter, k);
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

nodePos *searchNodePos(nodePos *list, int pos) {
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
                for(int i = 0; i < k - 1; i++)
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

void inserisciInizio(nodeTree **treeString, nodeTree **treeStringValid, nodeFilter *listFiltroSto, int lengthBuff, int k) {
    char* buffer = malloc(sizeof(char) * lengthBuff);

    while(1){
        if(scanf("%[^\n]s", buffer) == EOF) exit(-2);
        while ((getchar()) != '\n');
        if(strcmp(buffer, "+inserisci_fine") == 0) break;
        insertTree(treeString, buffer, k);
        if(filtraParola(buffer, listFiltroSto, k) == true)
            insertTree(treeStringValid, buffer, k);
    }

    free(buffer);
}

void nuovaPartita(nodeTree **treeString, int lengthBuff, int k){
    char* buffer = malloc(sizeof(char) * lengthBuff);
    nodeTree *treeStringValid = NULL;
    nodeFilter *listFiltroRec = NULL;
    nodeFilter *listFiltroSto = NULL;
    int n;

    if(scanf("%[^\n]s", buffer) == EOF) exit(-3);
    while ((getchar()) != '\n');

    nodeTree *pR = searchBST(*treeString, buffer);
    treeStringValid = createTreeValidRec(*treeString, NULL, k);

    if(scanf("%d\n", &n) == EOF) exit(-1);

    while(n > 0){
        if(scanf("%[^\n]s", buffer) == EOF) exit(-2);
        while ((getchar()) != '\n');

        if(strcmp(buffer, "+stampa_filtrate") == 0){
            inOrderTraverseTreeValid(treeStringValid);
            continue;
        }

        if(strcmp(buffer, "+inserisci_inizio") == 0){
            inserisciInizio(treeString, &treeStringValid, listFiltroSto, lengthBuff, k);
            //creazioneParole(treeString, lengthBuff, k, "+inserisci_fine");
            //deleteAllTree(treeStringValid);
            //treeStringValid = createTreeValidRec(*treeString, NULL, k);
            filtraStringhe(&treeStringValid, listFiltroSto, k);
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
            filtraStringhe(&treeStringValid, listFiltroRec, k);
            printf("%d\n", numeroStringheValide(treeStringValid));
            freeListaFiltro(&listFiltroRec);
        }
        else
            printf("not_exists\n");

        if(n == 0)
            printf("ko\n");
    }

    deleteAllTree(treeStringValid);
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