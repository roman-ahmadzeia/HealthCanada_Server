// Roman Ahmad Zeia - 100821974 - Roman Ahmad Zeia
// Socket Programming - Health Benefits Canada Mock Application
// Client
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_LEN 1024

void clear_input_buffer() { // method to clear input buffer to prevent bugs
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() // main method
{
    int client_socket;
    struct sockaddr_in server_addr;
   
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket Creation Failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    printf("Client Socket Initialized\n\n");


    char greeting[1024];
    recv(client_socket, greeting, MAX_LEN, 0);
    printf("%s", greeting);

    char name[1024];
    printf("Enter Name: ");
    scanf("%s", name);
    clear_input_buffer();    
    send(client_socket, name, strlen(name), 0);
    printf("Connection Established\n\n");

    while(1) // while loop to keep asking user if they want to recieve more information till termination
    {
        char greeting_name[1024];
        recv(client_socket, greeting_name, MAX_LEN, 0);
        printf("%s", greeting_name);
    	char menu[1024];
    	recv(client_socket, menu, 1024, 0);
        printf("%s", menu);

        char choice[MAX_LEN];
        printf("Please enter your choice:");
	    scanf("%s", choice);
        clear_input_buffer();
	    send(client_socket, choice, sizeof(choice), 0);

	    char response[1024];
	    recv(client_socket, response, sizeof(response),0);
	    printf("%s",response);
	    printf("Do you want more information? (y/n): ");
	    char more_info[MAX_LEN];
	    scanf("%s", more_info);
        clear_input_buffer();
        send(client_socket, more_info, sizeof(more_info), 0);
        if (strcmp(more_info, "n") == 0) {
            printf("%s disconnected.\n", name);
            break;
        }

       
    }

    return 0;
}







