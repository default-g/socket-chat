#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>


int server_socket;
int clients[5];
int connection_amount;
pthread_mutex_t mutex;
char welcome_message[256] = "Welcome to chat! (´｡• ᵕ •｡`) ♡ \n";


void send_messages(int from, char *message) {
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < 5; i++) {
        if (from != clients[i]) {
            send(clients[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}


void* client(void* arg) {
    int socket = *((int *) arg);
    char message[256];
    int len;
    while((len = recv(socket, message, sizeof(message), 0)) > 0) {
        
        message[len] = '\0';
        send_messages(socket, message);
    }
    pthread_mutex_lock(&mutex);
    connection_amount--;
    pthread_mutex_unlock(&mutex);
}


int main() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int bind_result = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    listen(server_socket, 5);

    pthread_t receiver;

    int client_socket = 0;

    while(1) {
        if ((client_socket = accept(server_socket, NULL, NULL)) < 0) {
            printf("Connection failed!\n");
        }    
        pthread_mutex_lock(&mutex);

        clients[connection_amount++] = client_socket;
        printf("Connections : %d \n", connection_amount);
        send(client_socket, welcome_message, strlen(welcome_message), 0);
        pthread_create(&receiver, NULL, client, &client_socket);

        pthread_mutex_unlock(&mutex);
    }

    // close(server_socket);
    
    return 0;
}