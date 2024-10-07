#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "up_bst.h"


#define PORT 8080
#define MAX_CLIENTs 10

struct input{
    void* data;
    int key;
    int size;
};

int main(){

    int sock_fd, new_socket;
    struct sockaddr_in address;
    // void* buff = malloc(1024);
    int data_size, val_read;
    int num_pk[2];
    int addrlen = sizeof(address);

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0){
        perror("Socket Connection Failed\n");
        exit(EXIT_FAILURE);
    }
    
    if((bind(sock_fd, (struct sockaddr*)&address, addrlen) < 0)){
        perror("Bind Failed");
        exit(EXIT_FAILURE);
    } 

    if((listen(sock_fd,5))<0){
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }

    if((new_socket = accept(sock_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
        perror("Can't accept a connection.");
        exit(EXIT_FAILURE);
    }

    int fd = open("file_store", O_CREAT | O_WRONLY);

    struct node* head = NULL;
    while((val_read = recv(new_socket, num_pk, sizeof(num_pk), 0))>0){
        struct input *id = malloc(sizeof(struct input));
        void* buff = malloc(num_pk[0]); // the problem was in the declaration of buff outside the loop and since we declare the buff ptr outside the loop and since it is only a ref to the actual data and since we are actually resetting the buff at the end of every loop, the nodes which contains the refs to buff are also resetted leading to the data attribute of struct node, pointing to nothing as node->data points to buff and if buff has nothing so does node->data T_T.
        printf("%s : Data size: %d\n",inet_ntoa(address.sin_addr), num_pk[0]);
        recv(new_socket, buff, num_pk[0], 0);
        write(fd, &num_pk[1], 4);
        // printf("%ld\n", strlen(buff)); // print
        
        // // id->data = (char*)buff; // check
        // // id->key = num_pk[1];
        // // id->size = num_pk[0];

        // // printf("%s", (char *) id->data); //print

        write(fd, buff, num_pk[0]);

        head = insert(head,createNode(buff, num_pk[1], num_pk[0]));
        // printf(" Print after head : %s\n",(char*)head->data->data);

        // memset(buff, 0, 1024);
        memset(&data_size,0,4);
        // free(id);
        // free(buff); 


    }
    printf(" Print after loop : %s\n",(char*)head->data->data);
    printf("Tree after construction: \n");
    printTree(head);
    close(fd);
    fd = open("bst_file", O_CREAT | O_WRONLY, S_IRWXU);

    saveBst(fd, head, 3);

    close(fd);

    fd = open("bst_file", O_RDONLY);
    if (fd < 0) {
        perror("file");
        exit(1);
    }
    printf("Output after reading from the file: \n");
    readBst(fd,3);
    close(fd);

    printf("Tree after reconstruction from file: \n");

    fd = open("bst_file", O_RDONLY);
    if (fd < 0) {
        perror("file");
        exit(1);
    }

    printTree(recBst(fd, 3));

    close(fd);

    // free(buff); 
    close(fd);
    close(new_socket);
    close(sock_fd);
    return 0;
}