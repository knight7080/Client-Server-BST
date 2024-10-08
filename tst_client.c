#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

struct data {
    int key;
    char* data;
    int ds;
};


int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    char buff[1024];

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    // auto data feed, type independent data.

    // struct data key_data_list[] = {
    //     {5, "jack", 0},
    //     {9, "rose", 0},
    //     {3, "anna", 0}
    // };
    
    // int list_size = sizeof(key_data_list) / sizeof(key_data_list[0]);

    // for (int i = 0; i < list_size; i++) {
    //     struct data* n_data = &key_data_list[i];
    //     n_data->ds = strlen(n_data->data) + 1;

    //     int num_pk[] = {n_data->ds, n_data->key};
    //     send(sock_fd, num_pk, sizeof(num_pk), 0);
    //     send(sock_fd, n_data->data, n_data->ds, 0);

    //     printf("Sent Key: %d, Data: %s\n", n_data->key, n_data->data);
    //     sleep(1);
    // }

    // Manual Data Entry, using string for user demo input. Type independent aswell.

    while(1){
        struct data* n_data = malloc(sizeof(struct data));
        printf("Enter Key and Data: ");
        scanf("%d %s", &n_data->key, buff);
        n_data->data = buff;
        n_data->ds = strlen(buff);

        int num_pk[] = {n_data->ds, n_data->key};
        send(sock_fd, num_pk, sizeof(num_pk),0);
        send(sock_fd, n_data->data, n_data->ds, 0);
        memset(buff, 0, 1024);
    } 

    close(sock_fd);
    return 0;
}
