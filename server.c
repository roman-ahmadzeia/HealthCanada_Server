// Roman Ahmad Zeia - 100821974 - Romna Ahmad Zeia
// Socket Programming - Health Benefits Canada Mock Application
// Server
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define MAX_CLIENTS 5 // max number of clients that can use application at once
#define PORT 8080
#define MAX_LEN 1024

void call_log(char text[MAX_LEN])
{
    time_t now;
    struct tm *tm_info;
    time(&now);
    tm_info = localtime(&now);
    char timestamp[MAX_LEN];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    
    FILE *log_file = fopen("server_log.txt", "a");
    fprintf(log_file, "%s\n %s\n", timestamp, text);
    fclose(log_file);

}

void handle_menu_selection(int client_socket, int choice) {
    FILE *file;
    char line[1024];
    char *benefit_info = NULL;
    char *abbreviation = NULL;
    char *title = NULL;
    char *desc = NULL;
    char *link = NULL;
    file = fopen("Benefits_Canada.txt", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    while (fgets(line, sizeof(line), file) != NULL) {
        char *token = strtok(line, ";");
        int benefit_choice = atoi(token);

        if (benefit_choice == choice) {
            abbreviation = strtok(NULL, ";");
            title = strtok(NULL, ";");
            desc = strtok(NULL, ";");
            link = strtok(NULL, ";");
            break;
        }
    }
    fclose(file);

    char code_log[MAX_LEN];
    snprintf(code_log, sizeof(code_log),"%s information requested\n", abbreviation);
    call_log(code_log);

    if (abbreviation != NULL && title != NULL && desc != NULL && link != NULL) {
        char message[4096];
        char sent_log[MAX_LEN];
        snprintf(message, sizeof(message),
        "\nPlease find below information on %s\n\n%s(%s):\n%s\n\nPlease visit the following link for more information:\n%s\n\n",title,title,abbreviation,desc, link);
        snprintf(sent_log, sizeof(sent_log),"Requested Information Sent!\n");
        call_log(sent_log);
        send(client_socket, message, strlen(message), 0);
        printf("Requested Information Sent!\n");

    } else {
        char *error_message = "Benefit information not found for the selected choice\n";
        send(client_socket, error_message, strlen(error_message), 0);
    }
}

void *connect_client(void *arg) 
{
	int client_socket = *((int *)arg);

    char welcome_message[1024] = "Welcome To Benefits Canada Server!\n";
    send(client_socket, welcome_message, strlen(welcome_message),0);
    char log_welcome[MAX_LEN];
    snprintf(log_welcome, sizeof(log_welcome), "Welcome message sent!\n");
    call_log(log_welcome);
    
    char name[100];
    recv(client_socket, name, sizeof(name), 0);

    

    while(1)
    {
        char greeting_name[1024];
        snprintf(greeting_name, sizeof(greeting_name), "\nHello, %s\n", name);
        send(client_socket, greeting_name, sizeof(greeting_name), 0);
        char menu[1024] =  "To get information about benefits, please select from the following:\n 1. Canada Workers Benefit\n 2. Canada Child Benefits\n 3. Registered Disability Savings Plan\n 4. War Veterans Allowance\n 5. Disability Tax Credit\n 6. Canada Dental Benefits\n\n"; 
        send(client_socket, menu, strlen(menu), 0);
        char choice_c[1024];
        recv(client_socket, choice_c, MAX_LEN, 0);
        int choice = atoi(choice_c);
        handle_menu_selection(client_socket, choice);

        char more_info_choice[1024];
        recv(client_socket, more_info_choice, MAX_LEN, 0);
        if (strcmp(more_info_choice, "n") == 0) {
            printf("%s disconnected.\n", name);
            break;
        }


    }
    
    close(client_socket);
    free(arg);
    pthread_exit(NULL);
}	

int main()
{
    int server_socket;
    int client_socket;
    int addr_len;
    struct sockaddr_in server_addr, client_addr;
    pthread_t *threads[MAX_CLIENTS];
    int *new_client_socket;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket Creation failed");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) { 
       perror("Socket binding failed");
       exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Socket listening failed");
        exit(EXIT_FAILURE);
    }
    printf("Server Started\n");

     while (1) {
        addr_len = sizeof(struct sockaddr_in);
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len)) == -1) {
            perror("Connection accepting failed");
            exit(EXIT_FAILURE);
        }
        printf("Listening to incoming connection\n");

        new_client_socket = (int *)malloc(sizeof(int));
        if (new_client_socket == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        *new_client_socket = client_socket;

        threads[MAX_CLIENTS] = (pthread_t *)malloc(sizeof(pthread_t));
        if (threads[MAX_CLIENTS] == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        if (pthread_create(threads[MAX_CLIENTS], NULL, connect_client, (void *)new_client_socket) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    close(server_socket); 
    return 0;

}








