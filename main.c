#include<fcntl.h>
#include<stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include "up_bst.h"
struct input{
    void* data;
    int key;
    int size;
};

int main(){
    struct input arr[] = {
        {"John",5,sizeof("John")},
        {"James",10,sizeof("James")},
        {"Jack",2,sizeof("Jack")},
        {"Jill",9,sizeof("Jill")},
        {"Jam",22,sizeof("Jam")}
    };
    struct node* head = NULL;
    for(int i = 0; i<5; i++){
        head = insert(head, createNode(arr[i].data,arr[i].key,arr[i].size));
    }
    
    printf("Tree after construction: \n");
    printTree(head);

    int fd = open("bst_file", O_CREAT | O_WRONLY);

    saveBst(fd, head, 5);

    close(fd);

    fd = open("bst_file", O_RDONLY);
    if (fd < 0) {
        perror("file");
        exit(1);
    }
    printf("Output after reading from the file: \n");
    readBst(fd,5);
    close(fd);

    printf("Tree after reconstruction from file: \n");

    fd = open("bst_file", O_RDONLY);
    if (fd < 0) {
        perror("file");
        exit(1);
    }

    printTree(recBst(fd, 5));

    close(fd);

}