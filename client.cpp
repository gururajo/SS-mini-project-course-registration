#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
using namespace std;

#define BUF_SIZE 1024
int clientSocketfd;
bool echo=true;

void reset_str(char *str)
{
    for (int i = 0; i < BUF_SIZE; i++)
    {
        // cout << "f";
        str[i] = '\0';
    }
}
void *readFromServer(void *)
{
    char buffer[BUF_SIZE];
    while (true)
    {
        reset_str(buffer);
        // cout << "b" << endl;
        ssize_t bytesRead = read(clientSocketfd, buffer, sizeof(buffer));
        // cout << "a" << endl;
        if (bytesRead == -1)
        {
            perror("Error reading data from server");
            break;
        }

        if (bytesRead == 0)
        {
            cout << "Server closed the connection." << endl;
            break;
        }
        // if (strstr(buffer, "Password:") != NULL)
        // {
        //     system("stty -echo");
        // }
        // cout << "Received: " << bytesRead << endl;
        // buffer[bytesRead] = '\0'; // Null-terminate the received data
        cout << buffer;
    }
    return nullptr;
}

void *cinfromuser(void *)
{
    char userInput[1024];
    while (true)
    {
        // cout<<"readyto read"<<endl;
        // cin >> userInput;
        cin.getline(userInput, sizeof(userInput));
        // if(!echo){
        //     system("stty echo");
        // }
        // cout<<"GIT:"<<userInput<<endl;
        if (strcmp(userInput, "quit") == 0)
        {
            break; // Exit the loop
        }

        ssize_t bytesSent = write(clientSocketfd, userInput, strlen(userInput));
        if (bytesSent == -1)
        {
            perror("Error sending data to server");
            break;
        }
    }
    return nullptr;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <server_ip> <server_port>" << endl;
        return 1;
    }

    const char *serverIp = argv[1];
    int serverPort = atoi(argv[2]);
    // Create a socket
    clientSocketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocketfd == -1)
    {
        perror("Error creating socket");
        return 1;
    }

    // Server address and port
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);          // Change this to the server's port
    serverAddr.sin_addr.s_addr = inet_addr(serverIp); // Change this to the server's IP address

    // Connect to the server
    if (connect(clientSocketfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Error connecting to server");
        return 1;
    }

    // Mpthread_t threadID;
    pthread_t threadIDread, threadIDcin;
    pthread_create(&threadIDread, NULL, readFromServer, NULL);
    pthread_create(&threadIDcin, NULL, cinfromuser, NULL);

    // Close the socket

    pthread_join(threadIDread, NULL);
    pthread_join(threadIDcin, NULL);
    close(clientSocketfd);

    return 0;
}
