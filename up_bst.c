#include "up_bst.h"

struct Data {
    void* data;
    size_t ds;
    int depth;
};

struct node* createNode(void* data, int key, int size){
    struct node *n = malloc(sizeof(struct node));
    anydata *d = malloc(sizeof(anydata));
    d->data = data;
    d->ds = size;
    n->data = d;
    n->key = key;
    n->l = NULL;
    n->r = NULL;
    n->depth = 0;
    // printf("\nIn CreateNode:%s\n", (char* )d->data); // print
    return n;
}

struct node* insert(struct node* head, struct node* n){
    struct node* curr_ptr = head;
    if(head == NULL){
        head = n;
    }
    else{
        while(curr_ptr != NULL){
            if(curr_ptr->key < n->key && curr_ptr->r != NULL){
                curr_ptr = curr_ptr->r;
                n->depth+=1;
            }
            else if (curr_ptr->key > n->key && curr_ptr->l != NULL){ 
                curr_ptr = curr_ptr->l;  
                n->depth+=1;
            }
            else{
                n->depth+=1;
                break;
            }
        }
        if(curr_ptr->key < n->key){
            curr_ptr->r = n;
        }
        else{
            curr_ptr->l = n;
        }
    }
    return head;
}

// I shall let you live.
void printTree(struct node* head){
    if(head == NULL){
        return; 
    }
    printf("Key : %d, Data : %s \n", head->key, (char*)head->data->data);
    printTree(head->l);
    printTree(head->r);
}

void saveBst(int fd, struct node* head, int size) {
    struct node** stack = (struct node**)malloc(size * sizeof(struct node*));
    int curr_node = 0;

    while (curr_node > 0 || head != NULL) {
        while (head != NULL) {
            stack[curr_node++] = head;
            head = head->l;
        }

        if (curr_node > 0) {
            head = stack[--curr_node];
            int buff[] = {head->key, head->data->ds, head->depth};
            write(fd,buff,sizeof(buff));
            // printf("%s\n", (char* )head->data->data); //
            write(fd, head->data->data,head->data->ds);
            head = head->r;
        }
    }

    free(stack);
}

void readBst(int fd, int size){
    for (int i = 0; i < size; i++) {
        int inp_buff[3];
        read(fd, inp_buff, sizeof(inp_buff));
        
        struct Data* data = (struct Data*)malloc(sizeof(struct Data));
        data->ds = inp_buff[1];
        data->data = malloc(data->ds);
        read(fd, data->data, data->ds);
        
        printf("Key: %d, Data: %s\n", inp_buff[0], (char*)data->data);

    }   
}

// int comparator(const void *a, const void* b){  
//     struct node temp_a = *((struct node*)a);
//     struct node temp_b = *((struct node*)b);
//     if(temp_a.depth > temp_b.depth){
//         return 1;
//     }
//     else if(temp_a.depth < temp_b.depth){
//         return -1;
//     }
//     else{
//         return 0;
//     }
// }

int comparator(const void *a, const void *b){  
    struct node* temp_a = *(struct node**)a;
    struct node* temp_b = *(struct node**)b;
    if(temp_a->depth > temp_b->depth){
        return 1;
    }
    else if(temp_a->depth < temp_b->depth){
        return -1;
    }
    else{
        return 0;
    }
}

struct node* recBst(int fd, int size){
    struct node* node_arr[size];
    struct node* head = NULL;
    for (int i = 0; i < size; i++) {
        int inp_buff[3];
        read(fd, inp_buff, sizeof(inp_buff));

        struct Data* data = (struct Data*)malloc(sizeof(struct Data));
        data->ds = inp_buff[1];
        data->data = malloc(data->ds);
        read(fd, data->data, data->ds);

        node_arr[i] = createNode(data->data,inp_buff[0],data->ds);
        node_arr[i]->depth = inp_buff[2];
    }

    qsort(node_arr, size, sizeof(struct node*), comparator);
    
    for(int i = 0; i<size; i++){
        head = insert(head, node_arr[i]);
    }
    return head;
}