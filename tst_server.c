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
    int data_size, val_read;
    int num_pk[2];
    int addrlen = sizeof(address);
    int node_count = 0;

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    inet_pton(AF_INET, "172.26.199.112", &address.sin_addr);
    // address.sin_addr.s_addr = INADDR_ANY;

    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0){
        perror("Socket Connection Failed\n");
        exit(EXIT_FAILURE);
    }
    
    if((bind(sock_fd, (struct sockaddr*)&address, addrlen) < 0)){
        perror("Bind Failed");
        exit(EXIT_FAILURE);
    } 
    printf("Waiting for connection....\n");
    if((listen(sock_fd,5))<0){
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }

    if((new_socket = accept(sock_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
        perror("Can't accept a connection.");
        exit(EXIT_FAILURE);
    }
    printf("Connected to %s\n", inet_ntoa(address.sin_addr));
    int fd = open("file_store", O_CREAT | O_WRONLY);

    struct node* head = NULL;
    while(1){

        int status = recv(new_socket, num_pk, sizeof(num_pk), 0);

        if(status <= 0 ){
            printf("Connection Closed. Waiting for a new one...\n");
            new_socket = accept(sock_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            printf("Connected to %s\n", inet_ntoa(address.sin_addr));
            continue;
        }

        struct input *id = malloc(sizeof(struct input));

        void* buff = malloc(num_pk[0]); 

        printf("%s : Data size: %d\n",inet_ntoa(address.sin_addr), num_pk[0]);
        
        recv(new_socket, buff, num_pk[0], 0);

        write(fd, &num_pk[1], 4);
        write(fd, buff, num_pk[0]);

        head = insert(head,createNode(buff, num_pk[1], num_pk[0]));

        node_count+=1;
    }

    printf("Tree after construction: \n");
    printTree(head);
    close(fd);
    fd = open("bst_file", O_CREAT | O_WRONLY, S_IRWXU);

    saveBst(fd, head, node_count);

    close(fd);

    fd = open("bst_file", O_RDONLY);
    if (fd < 0) {
        perror("file");
        exit(1);
    }
    printf("Output after reading from the file: \n");
    readBst(fd,node_count);
    close(fd);

    printf("Tree after reconstruction from file: \n");

    fd = open("bst_file", O_RDONLY);
    if (fd < 0) {
        perror("file");
        exit(1);
    }

    printTree(recBst(fd, node_count));

    close(fd);
    close(new_socket);
    close(sock_fd);

    printf("Connection terminated.\n");
    return 0;
}