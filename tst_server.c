#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h> 
#include <pthread.h>
#include "up_bst.h"

#define PORT 8080

struct input {
    void* data;
    int key;
    int size;
};

int sock_fd;
pthread_mutex_t tree_mutex = PTHREAD_MUTEX_INITIALIZER;  
struct node* shared_tree = NULL;  
int shared_node_count = 0;

void* handleClient(void* arg);
void processData(struct sockaddr_in addr);
void handleClose(int signum);

int main() {
    signal(SIGINT, handleClose);
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
        perror("Socket Connection Failed\n");
        exit(EXIT_FAILURE);
    }

    if ((bind(sock_fd, (struct sockaddr*)&address, addrlen) < 0)) {
        perror("Bind Failed");
        exit(EXIT_FAILURE);
    }
    printf("Waiting for connection....\n");

    if ((listen(sock_fd, 5)) < 0) {
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int new_socket = accept(sock_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("Can't accept a connection.");
            exit(EXIT_FAILURE);
        }

        printf("Connected to %s\n", inet_ntoa(address.sin_addr));

        pthread_t thread_id;
        int* client_socket = malloc(sizeof(int));
        *client_socket = new_socket;

        if (pthread_create(&thread_id, NULL, handleClient, (void*)client_socket) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }

        pthread_detach(thread_id);  
    }

    close(sock_fd);
    printf("Server terminated.\n");
    return 0;
}

void* handleClient(void* arg) {
    int new_socket = *(int*)arg;
    free(arg);  

    struct sockaddr_in address;
    int addrlen = sizeof(address);
    getpeername(new_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);

    int num_pk[2];

    while (1) {
        char msg[] = "Select a option \n 1. Insert Node\n 2. Delete Node\n 3. List Tree\n ";
        int choice, key;

        int status = recv(new_socket, &choice, sizeof(int), 0);

        if (status <= 0) {
            printf("Connection Closed. Exiting thread...\n\n");
            close(new_socket);
            pthread_exit(NULL);  
        }

        pthread_mutex_lock(&tree_mutex);

        if (choice == 1) {
            recv(new_socket, num_pk, sizeof(num_pk), 0);
            void* buff = malloc(num_pk[0]);

            printf("%s : Data size: %d\n", inet_ntoa(address.sin_addr), num_pk[0]);

            recv(new_socket, buff, num_pk[0], 0);

            shared_tree = insert(shared_tree, createNode(buff, num_pk[1], num_pk[0]));
            shared_node_count += 1;

            processData(address);
        }

        if (choice == 2) {
            recv(new_socket, &key, sizeof(key), 0);

            struct node* temp = delete(shared_tree, key);
            if (temp == NULL) {
                char res[] = "The key doesn't exist in the tree";
                send(new_socket, res, sizeof(res), 0);
            } else {
                shared_tree = temp;
                char res[] = "The element with the given key is removed.";
                send(new_socket, res, sizeof(res), 0);
                shared_node_count -= 1;

                processData(address);

                char* string_tree = generateTreeString(shared_tree);
                send(new_socket, string_tree, strlen(string_tree), 0);  
                free(string_tree);
            }
        }


        if (choice == 3) {
            char* string_tree = malloc(1024);
            string_tree = generateTreeString(shared_tree);
            send(new_socket, string_tree, strlen(string_tree), 0);
            free(string_tree);
        }

        pthread_mutex_unlock(&tree_mutex);
    }
}

void processData(struct sockaddr_in addr) {
    printf("Tree after construction: \n");
    printTree(shared_tree);

    int fd = open("shared_bst_file", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (fd < 0) {
        perror("Failed to open the shared file");
        return;
    }

    saveBst(fd, shared_tree, shared_node_count);
    close(fd);

    fd = open("shared_bst_file", O_RDONLY);
    if (fd < 0) {
        perror("Failed to read the shared file");
        return;
    }

    printf("Output after reading from the file: \n");
    readBst(fd, shared_node_count);
    close(fd);

    printf("Tree after reconstruction from file: \n");
    fd = open("shared_bst_file", O_RDONLY);
    if (fd < 0) {
        perror("Failed to reopen the file");
        return;
    }

    printTree(recBst(fd, shared_node_count));
    close(fd);
}

void handleClose(int signum) {
    close(sock_fd);
    exit(0);
}
