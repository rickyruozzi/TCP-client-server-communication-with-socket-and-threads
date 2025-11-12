#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char messageBuffer[1024];
int main(){
    printf("Client is running...\n");
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET; //ipv4
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(8080); //porta del server
    connect(client_fd, (struct sockaddr*)&client_addr, sizeof(client_addr));
    read(client_fd, messageBuffer, sizeof(messageBuffer));
    printf("Message from server: %s", messageBuffer);
    memset(messageBuffer, 0, sizeof(messageBuffer)); //clear del buffer
    while(1){

        printf("Enter message: ");
        fgets(messageBuffer, sizeof(messageBuffer), stdin);
        send(client_fd, messageBuffer, strlen(messageBuffer), 0);
        memset(messageBuffer, 0, sizeof(messageBuffer)); //clear del buffer
    }
    close(client_fd);
    return 0;
}