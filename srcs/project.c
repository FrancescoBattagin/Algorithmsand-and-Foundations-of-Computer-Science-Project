#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#define LEN_MAX 900
#define LEN_MIN 40
#define LEN_REL 30

typedef struct entities_s{
    char* source[LEN_MAX];         //elenco id entità sorgenti di questa rel per questa entità dest
    char* dest;                     //id entità dest per questa relazione
    int last_source_index;
    struct entities_s *next;
} entities_t;

typedef struct bst_s{
    int max;                        //max per questa relazione
    char* max_entities[LEN_MAX];   //elenco entità che hanno il max per questa relazione
    int last_max_entities;
    entities_t *ent;                //lista delle entità
    char* rel_id;
    struct bst_s *left;             //figlio sx
    struct bst_s *right;            //figlio dx
    struct bst_s *dad;
} bst_t;


void insert_ent_name(char **ent_detector, char temp_ent_name[], int last_ent_index);
void insert_rel_name(char **rel_detector, char temp_rel_name[], int last_rel_index);
void quick_sort(char **array, int first, int last);
int partition(char **array, int first, int last);
int binary_search(char **array, char *to_find, int first, int last);
bst_t* search_bst_rel(bst_t *root, char* temp_rel_id);
bst_t* insert_bst_rel(bst_t *root, char* temp_rel_id, char* dest_id, char* source_id);
void bst_walk(bst_t * root, char **rel_detector, int last_rel_index);
bst_t* delrel(bst_t* root, char* temp_rel_name, char* source_ent, char* dest_ent);
void delent(bst_t* root, char *temp_ent_name, char **ent_detector, int last_ent_index);
void search_bst_delent(bst_t* root, char *temp_ent_name);
void max_entities_update(bst_t *node);

int main (int argc, char *argv[]) {                 //index logic: every updated index is already the one which has to be used; once used, it is incremented
    char command[7];
    char c;
    char temp_rel_name[LEN_MIN + 1];
    char temp_ent_name1[LEN_MIN + 1];
    char temp_ent_name2[LEN_MIN + 1];
    char **rel_detector;
    char **ent_detector;
    int last_ent_index = 0;
    int last_rel_index = 0;
    bst_t *rel_bst = NULL;

    //freopen("input_Suite6.2", "r", stdin);

    //Malloc array di stringhe per entità e relazioni
    ent_detector = (char **) malloc(sizeof(char *) * LEN_MAX);
    rel_detector = (char **) malloc(sizeof(char *) * LEN_REL);
    for (int i = 0; i < LEN_MAX; i++) {
        ent_detector[i] = (char *) malloc(sizeof(char) * (LEN_MIN + 1));
        rel_detector[i] = (char *) malloc(sizeof(char) * (LEN_REL + 1));
    }


    //Ciclo di esecuzione
    scanf("%s", command);                           //command
    while (command[0] != 'e') {                      //se il comando è diverso da 'end'
        if (command[0] == 'a') {
            if (command[3] == 'e') {                //addent
                int found = 0;
                scanf("%c", &c); // ' '
                scanf("%s", temp_ent_name1);
                found = binary_search(ent_detector, temp_ent_name1, 0, last_ent_index - 1); //check se esiste già un'entità con lo stesso nome
                if (!found) {
                    insert_ent_name(ent_detector, temp_ent_name1, last_ent_index);
                    last_ent_index++;
                }
            } else {                                  //addrel
                if (command[3] == 'r') {              //check
                    int found_rel = 0;
                    int found_ent_1 = 0;
                    int found_ent_2 = 0;
                    scanf("%c", &c); // ' '
                    scanf("%s", temp_ent_name1);
                    found_ent_1 = binary_search(ent_detector, temp_ent_name1, 0, last_ent_index - 1);
                    if (found_ent_1) {
                        scanf("%c", &c); // ' '
                        scanf("%s", temp_ent_name2);
                        found_ent_2 = binary_search(ent_detector, temp_ent_name2, 0, last_ent_index - 1);
                        if (found_ent_2) {
                            scanf("%c", &c); // ' '
                            scanf("%s", temp_rel_name);
                            if(last_rel_index == 0)
                                found_rel = 0;
                            else
                                found_rel = binary_search(rel_detector, temp_rel_name, 0, last_rel_index - 1);

                            if(!found_rel){ //new node
                                insert_rel_name(rel_detector, temp_rel_name, last_rel_index);
                                last_rel_index++;
                                rel_bst = insert_bst_rel(rel_bst, temp_rel_name, temp_ent_name2, temp_ent_name1);
                            }
                            else{ //update an existing node
                                bst_t *temp_root = rel_bst;
                                bst_t *temp = search_bst_rel(temp_root, temp_rel_name);
                                entities_t *temp_ent = temp->ent;
                                int exit = 0;

                                if(temp->ent == NULL){
                                    temp->ent = (entities_t*) malloc (sizeof(entities_t));
                                    temp->ent->dest = (char*) malloc (sizeof(char)*LEN_MIN);
                                    strcpy(temp->ent->dest, temp_ent_name2);
                                    for(int i = 0; i < LEN_MAX; i++)
                                        temp->ent->source[i] = (char*) malloc (sizeof(char)*LEN_MIN);
                                    strcpy(temp->ent->source[0], temp_ent_name1);
                                    temp->ent->last_source_index = 1;
                                    if(temp->max == 1){
                                        strcpy(temp->max_entities[temp->last_max_entities], temp_ent_name2);
                                        temp->last_max_entities++;
                                    }
                                    else if(temp->max == 0){
                                        strcpy(temp->max_entities[0], temp_ent_name2);
                                        temp->last_max_entities++;
                                        temp->max++;
                                    }
                                    temp->ent->next = NULL;
                                }

                                else{
                                    while(!exit){
                                        if(/*temp_ent!= NULL && */!strcmp(temp_ent->dest, temp_ent_name2)){
                                            int found = 0;
                                            for(int i = 0; i < temp_ent->last_source_index && !found; i++)
                                                if(!strcmp(temp_ent->source[i], temp_ent_name1))
                                                    found = 1;
                                            if(!found){
                                                strcpy(temp_ent->source[temp_ent->last_source_index], temp_ent_name1);
                                                temp_ent->last_source_index++;
                                                //temp_ent->num++;
                                                if(temp->max < temp_ent->last_source_index){
                                                    temp->max = temp_ent->last_source_index;
                                                    for(int i = 1; i < temp->last_max_entities; i++)
                                                        strcpy(temp->max_entities[i], "");
                                                    strcpy(temp->max_entities[0], temp_ent->dest);
                                                    temp->last_max_entities = 1;
                                                }
                                                else if (temp->max == temp_ent->last_source_index){
                                                    strcpy(temp->max_entities[temp->last_max_entities], temp_ent->dest);
                                                    temp->last_max_entities++;
                                                }
                                            }
                                            quick_sort(temp->max_entities, 0, temp->last_max_entities-1);
                                            exit = 1;
                                        }
                                        else /*if(temp_ent!=NULL)*/{
                                            if(temp_ent->next!=NULL)
                                                temp_ent = temp_ent->next;
                                            else{
                                                temp_ent->next = (entities_t *) malloc (sizeof(entities_t));
                                                temp_ent->next->dest = (char*) malloc (sizeof(char)*LEN_MIN);
                                                strcpy(temp_ent->next->dest, temp_ent_name2);
                                                temp_ent->next->last_source_index = 1;
                                                for(int i = 0; i < LEN_MAX; i++)
                                                    temp_ent->next->source[i] = (char*) malloc (sizeof(char)*LEN_MIN);
                                                strcpy(temp_ent->next->source[0], temp_ent_name1);
                                                temp_ent->next->last_source_index = 1;
                                                temp_ent->next->next=NULL;
                                                if(temp->max == 0){
                                                    temp->max = 1;
                                                    strcpy(temp->max_entities[0], temp_ent->next->dest);
                                                    temp->last_max_entities = 1;
                                                }
                                                else if (temp->max == 1){
                                                    strcpy(temp->max_entities[temp->last_max_entities], temp_ent->next->dest);
                                                    temp->last_max_entities++;
                                                }
                                                quick_sort(temp->max_entities, 0, temp->last_max_entities-1);
                                                exit = 1;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else{
            if(command[0] == 'r'){ //report
                bst_walk(rel_bst, rel_detector, last_rel_index);
            }
            else{
                if(command[0] == 'd'){
                    if(command[3] == 'r'){ //delrel
                        scanf("%c", &c); // ' '
                        scanf("%s", temp_ent_name1);
                        int found1 = binary_search(ent_detector, temp_ent_name1, 0 ,last_ent_index-1);
                        scanf("%c", &c); // ' '
                        scanf("%s", temp_ent_name2);
                        int found2 = binary_search(ent_detector, temp_ent_name2, 0 ,last_ent_index-1);
                        scanf("%c", &c); // ' '
                        scanf("%s", temp_rel_name);
                        int foundrel = binary_search(rel_detector, temp_rel_name, 0 ,last_rel_index-1);
                        if(found1 && found2 && foundrel)
                            delrel(rel_bst, temp_rel_name, temp_ent_name1, temp_ent_name2);
                    }
                    else{ //delent
                        scanf("%c", &c); // ' '
                        scanf("%s", temp_ent_name1);
                        if(binary_search(ent_detector, temp_ent_name1, 0, last_ent_index-1)){
                            bst_t *temp_root = rel_bst;
                            delent(temp_root, temp_ent_name1, ent_detector, last_ent_index);
                            last_ent_index--;
                        }
                    }
                }
            }
        }
        scanf("%s", command);                           //command
    }
    return 0;
}

void insert_ent_name(char **ent_detector, char *temp_ent_name, int last_ent_index) {
    strcpy(ent_detector[last_ent_index], temp_ent_name);
    quick_sort(ent_detector, 0, last_ent_index);
}

void insert_rel_name(char **rel_detector, char *temp_rel_name, int last_rel_index) {
    strcpy(rel_detector[last_rel_index], temp_rel_name);
    quick_sort(rel_detector, 0, last_rel_index);
}

void quick_sort(char **array, int first, int last) {
    int q;
    if (first < last) {
        q = partition(array, first, last);
        quick_sort(array, first, q - 1);
        quick_sort(array, q + 1, last);
    }
}

int partition(char **array, int first, int last) {

    char x[LEN_MIN];
    int i = first - 1;
    int j;
    char tmp[LEN_MIN];
    strcpy(x, array[last]);
    for (j = first; j < last; j++) {
        if (strcmp(array[j], x) < 0) {
            i++;
            strcpy(tmp, array[j]);
            strcpy(array[j], array[i]);
            strcpy(array[i], tmp);
        }
    }
    strcpy(tmp, array[i + 1]);
    strcpy(array[i + 1], array[last]);
    strcpy(array[last], tmp);
    return i + 1;
}

int binary_search(char **array, char *to_find, int first, int last) {
    if(last == -1)
        return 0;
    if (!strcmp(to_find, array[last]))
        return 1;
    else if((!strcmp(to_find, array[first])))
        return 1;
    if (strcmp(to_find, array[first]) < 0 || strcmp(to_find, array[last]) > 0)
        return 0;
    int index = (last + first) / 2;
    int res = strcmp(to_find, array[index]);
    if (res == 0)
        return 1;
    else {
        if ((last - first) == 0 || ((strcmp(to_find, array[index]) > 0 && strcmp(to_find, array[index + 1]) < 0))){
            return 0;
        }
        else {
            if (res > 0)
                first = index;
            else if (res < 0)
                last = index;
            return binary_search(array, to_find, first, last);
        }
    }
}

bst_t* insert_bst_rel(bst_t *root, char* temp_rel_id, char* dest_id, char* source_id){
    bst_t * temp = NULL;
    bst_t * temp_root = root;
    bst_t * new_node = (bst_t*) malloc(sizeof(bst_t));
    new_node->rel_id = (char*) malloc (sizeof(char)*LEN_MIN);
    new_node->ent = (entities_t*) malloc (sizeof(entities_t));

    while(temp_root != NULL){
        temp = temp_root;
        if(strcmp(temp_rel_id, temp_root->rel_id) < 0)
            temp_root = temp_root->left;
        else
            temp_root = temp_root->right;
    }
    new_node->dad = temp;
    if(temp == NULL){
        root = new_node;
    }
    else{
        if(strcmp(temp_rel_id, temp->rel_id) < 0)
            temp->left = new_node;
        else
            temp->right = new_node;
    }
    strcpy(new_node->rel_id, temp_rel_id);
    for(int i = 0; i < LEN_MAX; i++)
        new_node->max_entities[i] = (char*) malloc (sizeof(char)*LEN_MIN);
    new_node->ent = (entities_t*) malloc(sizeof(entities_t));
    new_node->ent->dest = (char*) malloc (sizeof(char)*LEN_MIN);
    new_node->max = 1;
    new_node->left = NULL;
    new_node->right = NULL;
    strcpy(new_node->max_entities[0], dest_id);
    new_node->last_max_entities = 1;
    //new_node->ent->num = 1;
    strcpy(new_node->ent->dest, dest_id);
    for(int i = 0; i < LEN_MAX; i++)
        new_node->ent->source[i] = (char*) malloc (sizeof(char)*LEN_MIN);
    strcpy(new_node->ent->source[0], source_id);
    new_node->ent->last_source_index = 1;
    new_node->ent->next = NULL;
    return root;
}

bst_t* search_bst_rel(bst_t *root, char* temp_rel_id){
    while(root != NULL && strcmp(root->rel_id, temp_rel_id) != 0){
        if(strcmp(temp_rel_id, root->rel_id) > 0)
            root = root->right;
        else
            root = root->left;
    }
    return root;
}

void bst_walk(bst_t * root, char **rel_detector, int last_rel_index) {
    int printed = 0;
    int exit = 0;
    int i = 0;
    int acapo = 0;
    for ( ; i < last_rel_index; i++) {
        bst_t *rel_node = search_bst_rel(root, rel_detector[i]);
        if(rel_node->ent!= NULL && rel_node->max > 0){
            if(printed && i != 0)
                fputs(" ", stdout);
            fputs(rel_detector[i], stdout);
            fputs(" ", stdout);
            for (int j = 0; j < rel_node->last_max_entities && !exit; j++) {
                if(strcmp(rel_node->max_entities[j], "") != 0){
                    printed = 1;
                    fputs(rel_node->max_entities[j], stdout);
                    fputs(" ", stdout);
                }
                else
                    exit = 1;
            }
            if(i == last_rel_index-1 || ((search_bst_rel(root, rel_detector[i+1])->ent == NULL || search_bst_rel(root, rel_detector[i+1])->max == 0) && i ==last_rel_index-2)){
                printf("%d;\n", rel_node->max);
                acapo = 1;
            }
            else
                printf("%d;", rel_node->max);

        }
    }
    if (!printed)
        puts("none");
    else if(!acapo)
        fputs("\n", stdout);
}

bst_t* delrel(bst_t* root, char* temp_rel_name, char* source_ent, char* dest_ent){
    bst_t *found_rel = search_bst_rel(root, temp_rel_name);
    int num_temp;
    int end = 0;
    int end2 = 0;
    if(found_rel!=NULL){
        //cerco l'entità dest a cui sottrarre quella source e num--
        //se il vecchio num == max della rel, vado a togliere dest dalle max entitities
        entities_t * temp_ent = found_rel->ent;
        if(temp_ent!=NULL){
            while(temp_ent->next!=NULL && !end){
                if(strcmp(temp_ent->dest, dest_ent) == 0){
                    int i;
                    int exit = 0;
                    for(i = 0; i < temp_ent->last_source_index && !exit; i++){
                        if(strcmp(temp_ent->source[i], source_ent) == 0 )
                            exit = 1;
                    }
                    i--;
                    if(exit){
                        for( ; i < temp_ent->last_source_index -1; i++)
                            strcpy(temp_ent->source[i], temp_ent->source[i+1]);
                        strcpy(temp_ent->source[i], "");
                        temp_ent->last_source_index--;

                        //updating num && eventually max
                        num_temp = temp_ent->last_source_index+1;
                        //temp_ent->num--;
                        if(num_temp == found_rel->max) {
                            if (found_rel->last_max_entities == 1) {
                                found_rel->max--;
                                strcpy(found_rel->max_entities[0], "");
                                found_rel->last_max_entities--;
                                max_entities_update(found_rel);
                            }
                            else{
                                end2 = 0;
                                for (int j = 0; j < found_rel->last_max_entities && !end2; j++) {
                                    if (strcmp(dest_ent, found_rel->max_entities[j]) == 0) {
                                        end2 = 1;
                                        strcpy(found_rel->max_entities[j], "");
                                        for( ; j < found_rel->last_max_entities -1; j++)
                                            strcpy(found_rel->max_entities[j], found_rel->max_entities[j+1]);
                                        strcpy(found_rel->max_entities[j], "");
                                        found_rel->last_max_entities--;
                                    }
                                }
                            }
                        }
                        end = 1;
                    }
                    else
                        temp_ent = temp_ent->next;
                }
                else
                    temp_ent = temp_ent->next;
            }
            if(!end){
                if(strcmp(temp_ent->dest, dest_ent) == 0){
                    int i;
                    int exit = 0;
                    for(i = 0; i < temp_ent->last_source_index && !exit; i++) {
                        if (strcmp(temp_ent->source[i], source_ent) == 0) {
                            exit = 1;
                            for (; i < temp_ent->last_source_index - 1; i++)
                                strcpy(temp_ent->source[i], temp_ent->source[i + 1]);
                            strcpy(temp_ent->source[i], "");
                            temp_ent->last_source_index--;

                            //updating num && eventually max
                            num_temp = temp_ent->last_source_index + 1;
                            //temp_ent->num--;
                            if (num_temp == found_rel->max) {
                                if (found_rel->last_max_entities == 1) {
                                    found_rel->max--;
                                    strcpy(found_rel->max_entities[0], "");
                                    found_rel->last_max_entities--;
                                    max_entities_update(found_rel);
                                } else {
                                    end2 = 0;
                                    for (int j = 0; j < found_rel->last_max_entities && !end2; j++) {
                                        if (strcmp(dest_ent, found_rel->max_entities[j]) == 0) {
                                            end2 = 1;
                                            strcpy(found_rel->max_entities[j], "");
                                            for (; j < found_rel->last_max_entities - 1; j++)
                                                strcpy(found_rel->max_entities[j], found_rel->max_entities[j + 1]);
                                            strcpy(found_rel->max_entities[j], "");
                                            found_rel->last_max_entities--;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return root;
}

void delent(bst_t* root, char *temp_ent_name, char **ent_detector, int last_ent_index){
    //delete from ent_detector -> strcpy to the left
    int i = 0;
    int exit = 0;
    for( ; i < last_ent_index && !exit; i++){
        if(strcmp(ent_detector[i], temp_ent_name) == 0){
            exit = 1;
            for( ; i != last_ent_index-1; i++) //check i--?
                strcpy(ent_detector[i], ent_detector[i+1]);
            strcpy(ent_detector[i], ""); //last_ent_index-- in mai
        }
    }
    //delent in the tree
    search_bst_delent(root, temp_ent_name);
}

void search_bst_delent(bst_t* root, char *temp_ent_name){
    bst_t *temp_root = root;
    int first_deleted = 0;
    if(temp_root != NULL) {
        entities_t* temp_ent = temp_root->ent;
        search_bst_delent(temp_root->left, temp_ent_name);

        //primo nodo ent di temp_root
        //temp_root->ent->dest == temp_ent_name?
        if(temp_ent!=NULL){
            if(strcmp(temp_ent->dest, temp_ent_name) == 0){
                int exit_for = 0;
                first_deleted = 1;
                temp_root->ent = temp_ent->next;
                int temp_ent_num = temp_ent->last_source_index;
                entities_t *to_delete = temp_ent;
                temp_ent = temp_ent->next;
                free(to_delete->dest);
                //free(to_delete->source);
                free(to_delete);

                if (temp_root->max == temp_ent_num) { //dest in max entities
                    int i;
                    for (i = 0; i < temp_root->last_max_entities && !exit_for; i++){
                        if(strcmp(temp_root->max_entities[i], temp_ent_name) == 0){
                            exit_for = 1;
                            for ( ; i < temp_root->last_max_entities - 1; i++)
                                strcpy(temp_root->max_entities[i], temp_root->max_entities[i + 1]);
                            strcpy(temp_root->max_entities[i], "");
                        }
                    }
                    temp_root->last_max_entities--;
                    if (strcmp(temp_root->max_entities[0], "") == 0) { //update max
                        temp_root->max--;
                        if(temp_root -> last_max_entities > 0)
                            temp_root->last_max_entities--;
                        if(temp_root->ent!=NULL)
                            max_entities_update(temp_root);
                    }
                }
            }
                //altrimenti la cerco tra le source di temp_root->ent
            else{
                int exit = 0;
                for(int i = 0; i < temp_ent->last_source_index && !exit; i++){
                    if(strcmp(temp_ent->source[i], temp_ent_name) == 0){
                        exit = 1;
                        int j = i;
                        for( ; j < temp_ent->last_source_index-1; j++)
                            strcpy(temp_ent->source[j], temp_ent->source[j+1]);
                        strcpy(temp_ent->source[j], "");
                        temp_ent->last_source_index--;

                        //check sul num e max
                        int temp_ent_num = temp_ent->last_source_index+1;
                        //temp_ent->num--;

                        //se la dest era una max entity la cancello shiftando le altre verso sx
                        if(temp_ent_num == temp_root->max){
                            if (temp_root->last_max_entities == 1) {
                                temp_root->max--;
                                temp_root->last_max_entities--;
                                strcpy(temp_root->max_entities[0], "");
                                max_entities_update(temp_root);
                            }

                            else {
                                int end2 = 0;
                                for (int x = 0; x < temp_root->last_max_entities && !end2; x++) {
                                    if (strcmp(temp_root->ent->dest, temp_root->max_entities[x]) == 0) {
                                        end2 = 1;
                                        strcpy(temp_root->max_entities[x], "");
                                        for (; x < temp_root->last_max_entities - 1; x++)
                                            strcpy(temp_root->max_entities[x], temp_root->max_entities[x + 1]);
                                        strcpy(temp_root->max_entities[x], "");
                                        temp_root->last_max_entities--;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if(temp_ent!=NULL && first_deleted){ //check sources del secondo (ad es se viene eliminato il primo nodo)
            int exit1 = 0;
            for(int i = 0; i < temp_ent->last_source_index && !exit1; i++){
                if(strcmp(temp_ent->source[i], temp_ent_name) == 0){
                    exit1 = 1;
                    int j = i;
                    for( ; j < temp_ent->last_source_index-1; j++)
                        strcpy(temp_ent->source[j], temp_ent->source[j+1]);
                    strcpy(temp_ent->source[j], "");
                    temp_ent->last_source_index--;

                    //check sul num e max
                    int temp_ent_num = temp_ent->last_source_index+1;
                    //temp_ent->num--;

                    //se la dest era una max entity la cancello shiftando le altre verso sx
                    if(temp_ent_num == temp_root->max){
                        if (temp_root->last_max_entities == 1) {
                            temp_root->max--;
                            temp_root->last_max_entities--;
                            strcpy(temp_root->max_entities[0], "");
                            max_entities_update(temp_root);
                        }

                        else {
                            int end2 = 0;
                            for (int x = 0; x < temp_root->last_max_entities && !end2; x++) {
                                if (strcmp(temp_root->ent->dest, temp_root->max_entities[x]) == 0) {
                                    end2 = 1;
                                    strcpy(temp_root->max_entities[x], "");
                                    for (; x < temp_root->last_max_entities - 1; x++)
                                        strcpy(temp_root->max_entities[x], temp_root->max_entities[x + 1]);
                                    strcpy(temp_root->max_entities[x], "");
                                    temp_root->last_max_entities--;
                                }
                            }
                        }
                    }
                }
            }
        }


        //tutti i nodi ent in mezzo (per ogni nodo guardo il successivo -> non si passa al secondo poiché si guarda il successivo) -> ultimo nodo (next == NULL) si esce dal ciclo e lo processo fuori dal while
        if(temp_ent!=NULL) {
            int finish = 0;
            int exit2 = 0;
            int first_time = 1;
            entities_t* before_the_last = NULL;
            before_the_last = temp_ent;
            while (!finish && temp_ent->next != NULL) {
                //primo nodo ent di temp_root
                //temp_root->ent->dest == temp_ent_name?
                if (strcmp(temp_ent->next->dest, temp_ent_name) == 0) {
                    int temp_ent_num = temp_ent->next->last_source_index;
                    entities_t* to_delete = temp_ent->next;
                    temp_ent->next = temp_ent->next->next;
                    free(to_delete->dest);
                    //free(to_delete->source);
                    free(to_delete);
                    if (temp_root->max == temp_ent_num) { //dest in max entities
                        int i;
                        for (i = 0; i < temp_root->last_max_entities && !exit2; i++){
                            if (strcmp(temp_root->max_entities[i], temp_ent_name) == 0) {
                                exit2 = 1;
                                for (; i < temp_root->last_max_entities - 1; i++)
                                    strcpy(temp_root->max_entities[i], temp_root->max_entities[i + 1]);
                                strcpy(temp_root->max_entities[i], "");
                            }
                        }
                        temp_root->last_max_entities--;
                        if (strcmp(temp_root->max_entities[0], "") == 0) { //update max
                            temp_root->max--;
                            if(temp_root->last_max_entities > 0)
                                temp_root->last_max_entities--;
                            max_entities_update(temp_root);
                        }
                    }
                }
                    //altrimenti la cerco tra le source di temp_root->ent
                else {
                    int exit3 = 0;
                    for (int i = 0; i < temp_ent->next->last_source_index && !exit3; i++) {
                        if (strcmp(temp_ent->next->source[i], temp_ent_name) == 0) {
                            exit3 = 1;
                            int j = i;
                            for (; j < temp_ent->next->last_source_index - 1; j++)
                                strcpy(temp_ent->next->source[j], temp_ent->next->source[j + 1]);
                            strcpy(temp_ent->next->source[j], "");
                            temp_ent->next->last_source_index--;


                            //check sul num e max
                            int temp_ent_num = temp_ent->next->last_source_index+1;
                            //temp_ent->next->num--;

                            //se la dest era una max entity la cancello shiftando le altre verso sx
                            if (temp_ent_num == temp_root->max) {
                                if (temp_root->last_max_entities == 1) {
                                    temp_root->max--;
                                    temp_root->last_max_entities--;
                                    strcpy(temp_root->max_entities[0], "");
                                    max_entities_update(temp_root);
                                } else {
                                    int end2 = 0;
                                    for (int x = 0; x < temp_root->last_max_entities && !end2; x++) {
                                        if (strcmp(temp_ent->next->dest, temp_root->max_entities[x]) == 0) {
                                            end2 = 1;
                                            strcpy(temp_root->max_entities[x], "");
                                            for (; x < temp_root->last_max_entities - 1; x++)
                                                strcpy(temp_root->max_entities[x], temp_root->max_entities[x + 1]);
                                            strcpy(temp_root->max_entities[x], "");
                                            temp_root->last_max_entities--;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if(temp_ent->next != NULL){
                        temp_ent = temp_ent->next;
                        if(!first_time){
                            before_the_last = before_the_last->next;
                            first_time=0;
                        }
                    }
                }
            }


            //ultimo nodo ent di temp_root
            //temp_root->ent->dest == temp_ent_name?
            if (strcmp(temp_ent->dest, temp_ent_name) == 0) {
                int temp_ent_num = temp_ent->last_source_index;
                before_the_last->next=NULL;
                free(temp_ent->dest);
                //free(temp_ent->source);
                free(temp_ent);
                if (temp_root->max == temp_ent_num) { //dest in max entities
                    int i;
                    for (i = 0; strcmp(temp_root->max_entities[i], temp_ent_name) != 0; i++);
                    if(i==0)
                        i=1; //then i-- -> i=0
                    for (i--; i < temp_root->last_max_entities - 1; i++)
                        strcpy(temp_root->max_entities[i], temp_root->max_entities[i + 1]);
                    strcpy(temp_root->max_entities[i], "");
                    temp_root->last_max_entities--;
                    if (strcmp(temp_root->max_entities[0], "") == 0) { //update max
                        temp_root->max--;
                        temp_root->last_max_entities--;
                        max_entities_update(temp_root);
                    }
                }
            }
            //altrimenti la cerco tra le source di temp_root->ent
            else {
                int exit4 = 0;
                for (int i = 0; i < temp_ent->last_source_index && !exit4; i++) {
                    if (strcmp(temp_ent->source[i], temp_ent_name) == 0) {
                        exit4 = 1;
                        int j = i;
                        for (; j < temp_ent->last_source_index - 1; j++)
                            strcpy(temp_ent->source[j], temp_ent->source[j + 1]);
                        strcpy(temp_ent->source[j], "");
                        temp_ent->last_source_index--;

                        //check sul num e max
                        int temp_ent_num = temp_ent->last_source_index+1;
                        //temp_ent->num--;

                        //se la dest era una max entity la cancello shiftando le altre verso sx
                        if (temp_ent_num == temp_root->max) {
                            if (temp_root->last_max_entities == 1) {
                                temp_root->max--;
                                temp_root->last_max_entities--;
                                strcpy(temp_root->max_entities[0], "");
                                max_entities_update(temp_root);
                            } else {
                                int end2 = 0;
                                for (int x = 0; x < temp_root->last_max_entities && !end2; x++) {
                                    if (strcmp(temp_root->ent->dest, temp_root->max_entities[x]) == 0) {
                                        end2 = 1;
                                        strcpy(temp_root->max_entities[x], "");
                                        for (; x < temp_root->last_max_entities - 1; x++)
                                            strcpy(temp_root->max_entities[x], temp_root->max_entities[x + 1]);
                                        strcpy(temp_root->max_entities[x], "");
                                        temp_root->last_max_entities--;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if(temp_root->ent == NULL)
            temp_root->max = 0;
        search_bst_delent(temp_root->right, temp_ent_name);
    }
}

void max_entities_update(bst_t* node) {
    bst_t* temp_node = node;
    entities_t * temp_ent = temp_node->ent;
    int max = node->max;
    int updated = 0;
    int exit_while = 0;
    //node->last_max_entities = 0;
    if(temp_ent !=NULL){
        while(!updated && !exit_while){
            while (temp_ent->next != NULL) {
                if (max == temp_ent->last_source_index) {
                    strcpy(node->max_entities[node->last_max_entities], temp_ent->dest);
                    node->last_max_entities++;
                    updated = 1;
                }
                else if(max < temp_ent->last_source_index){
                    for(int i = 1; i < temp_node->last_max_entities-1; i++)
                        strcpy(temp_node->max_entities[temp_node->last_max_entities], "");
                    strcpy(temp_node->max_entities[0], temp_ent->dest);
                    temp_node->max = 1;
                    temp_node->last_max_entities = 1;
                    updated = 1;
                }
                temp_ent = temp_ent->next;
            }
            if(!updated){
                if(max > 0){
                    max--;
                    node->max--;
                    temp_ent = temp_node->ent;
                }
                else
                    exit_while = 1;
            }
        }
        //ultimo nodo ent di node
        if (max == temp_ent->last_source_index) {
            strcpy(node->max_entities[node->last_max_entities], temp_ent->dest);
            node->last_max_entities++;
        }
        else if(max < temp_ent->last_source_index){
            for(int i = 1; i < temp_node->last_max_entities-1; i++)
                strcpy(temp_node->max_entities[temp_node->last_max_entities], "");
            strcpy(temp_node->max_entities[0], temp_ent->dest);
            temp_node->max=temp_ent->last_source_index;
            temp_node->last_max_entities = 1;
        }
        quick_sort(node->max_entities, 0, temp_node->last_max_entities-1);
    }
}
