#ifndef up_bst
#define up_bst

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
typedef struct{
    void* data;
    int ds;
}anydata;

struct node{
    int key;
    anydata* data;
    struct node* l;
    struct node* r;
    int depth;
};

struct node* createNode(void* data, int key, int size);

struct node* insert(struct node* head, struct node* n);

void saveBst(int fd, struct node* head, int size);

void readBst(int fd, int size);

void printTree(struct node* head);

struct node* recBst(int fd, int size);

#endif