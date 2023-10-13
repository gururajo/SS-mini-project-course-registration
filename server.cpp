#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080
#define BUF_SIZE 1024
#define MAX_CLIENTS 5

int main()
{
    int sockfd, clientfd;
    socklen_t client_size;

    struct sockaddr_in server_address, client_address;

    ssize_t read_bytes, write_bytes;
    char data_from_client[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Error creating socket");
        exit(0);
    }

    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address) == -1))
    {
        perror("Error binding socket with 8080");
        server_address.sin_port = htons(PORT + 1);
        if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
        {
            perror("Error binding socket with 8081");
            exit(0);
        }
        else
        {
            printf("%d\n", 8081);
        }
    }
    else
    {
        printf("%d\n", 8080);
    }

    if (listen(sockfd, MAX_CLIENTS) == -1)
    {
        perror("Error listening for connections");
        exit(0);
    }

    printf("Listening for connections on port ...\n");

    while (1)
    {
        client_size = sizeof(client_address);
        clientfd = accept(sockfd, (struct sockaddr *)&client_address, &client_size);
        if (clientfd == -1)
        {
            perror("Error accepting connection");
        }
        else
        {
            printf("Got the connection from %s:%d\n", inet_ntoa(client_address.sin_addr), client_address.sin_port);
            if (fork() == 0)
            {
                printf("Created a new process\n");
                // printf("%d\n", handle_client(clientfd, client_address));
                close(clientfd);
            }
            else
            {
                close(clientfd);
            }
        }
    }

    close(sockfd);

    return 0;
}
