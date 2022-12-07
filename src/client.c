#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "config.h"

int network_socket;

void* receive_messages(void* args) {
    char server_response[MESSAGE_SIZE];
    int len;
    while((len = recv(network_socket, &server_response, sizeof(server_response), 0)) > 0) {
        server_response[len] = '\0';
        fputs(server_response, stdout);
    }
}


int main(int argc,char *argv[]) {
    char client_name[100];
    strcpy(client_name, argv[1]);
	// create a socket
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	// specify an address for the socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	// check for error with the connection
	if (connection_status == -1){
		printf("There was an error making a connection to the remote socket \n");
        return -1;
	}
	
	pthread_t messages_receiver;

    pthread_create(&messages_receiver, NULL, receive_messages, NULL);

    char message[MESSAGE_SIZE];

    while(fgets(message, MESSAGE_SIZE, stdin) > 0) {
        char send_msg[MESSAGE_SIZE];
        strcpy(send_msg, client_name);
        strcat(send_msg, ": ");
        strcat(send_msg, message);
        // message[size - 1]='\0';
        write(network_socket, send_msg, sizeof(send_msg));
    }

    pthread_join(messages_receiver, NULL);

	// and then close the socket
	close(network_socket);

	return 0;
}