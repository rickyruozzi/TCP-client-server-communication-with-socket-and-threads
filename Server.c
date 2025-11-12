#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define Max_clients 100

typedef struct client{
    int fd; //socket file desciriptor
    char ip[16]; //client ip address
    int port; //client port number
}client; //struct che identifica un client connesso

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //mutex per la gestione concorrente dei client

client clients[Max_clients]; //array dei client connessi
int client_count = 0; //tracking del numero di client connessi al server

void* handleClient(void* arg);

int main(){
    printf("Server is running...\n");
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; //ipv4
    server_addr.sin_addr.s_addr = INADDR_ANY; //accetta ogni indirizzo
    server_addr.sin_port = htons(8080); //porta del server
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)); //associa il socket ad un indirizzo e ad una porta
    listen(server_fd, 10); //si mette in ascolto per le richieste in entrata
    while(1){
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr); //imposta la lunghezza dell'indirizzo del client
        int client_fd=accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len); //accetta una connessione in entrata
        if(client_fd <0){   //controlla se il socket è stato accettato correttamente
            perror("Thread not accepted.");
            exit(EXIT_FAILURE);
        }
        pthread_t tid; //thread id 
        pthread_mutex_lock(&mutex); //crea il mutex per la gestione concorrente.
        if(client_count < Max_clients){ //imposto i valori nella struttura dati del client
            inet_ntop(AF_INET, &client_addr.sin_addr, clients[client_count].ip, sizeof(clients[client_count].ip)); //converte l'indirizzo IP del client 
            clients[client_count].port = ntohs(client_addr.sin_port); //imposta la porta del client
            clients[client_count].fd = client_fd; //imposta il file descriptor del client
            client_count++; //incremento della posizione del client
        } else {
            close(client_fd); //sono già connessi troppi client e quindi chiudo la connessione
        }
        pthread_mutex_unlock(&mutex);
        pthread_create(&tid, NULL, handleClient, (void*)&clients[client_count - 1]);
        // il socket è dentro la struttura dati del client insieme alle informazioni di riconoscimento
        pthread_detach(tid); //serve per evitare memory leak, il detach "stacca" il thread creato da quello iniziale
    }
    close(server_fd);
    return 0;
}

void* handleClient(void* arg){
    client* client_info = (client*)arg;
    char message[1024];
    char string[1024] = "Message from server: Hello Client!\n ";
    send(client_info->fd, string , sizeof(string), 0);
    while(1){
        ssize_t bytes_received = recv(client_info->fd, message, sizeof(message), 0);
        if(bytes_received <= 0){
            perror("Client disconnected or error occurred.");
            close(client_info->fd);
            break;
        }
        message[bytes_received] = '\0'; //aggiunge il terminatre di stringa
        printf("Received from %s:%d: %s\n", client_info->ip, client_info->port, message);
    }
    return NULL;
}