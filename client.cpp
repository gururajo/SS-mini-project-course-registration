#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
using namespace std;
int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port>" << std::endl;
        return 1;
    }

    const char* serverIp = argv[1];
    int serverPort = std::atoi(argv[2]);
    // Create a socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating socket");
        return 1;
    }

    // Server address and port
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); // Change this to the server's port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change this to the server's IP address

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to server");
        return 1;
    }

    // Make the socket non-blocking
    int flags = fcntl(clientSocket, F_GETFL, 0);
    fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);

    char buffer[1024];
    char userInput[1024];

    while (true) {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(STDIN_FILENO, &readSet);
        FD_SET(clientSocket, &readSet);

        // Use select to monitor both stdin and the socket for reading
        int maxFd = max(STDIN_FILENO, clientSocket) + 1;
        int ready = select(maxFd, &readSet, NULL, NULL, NULL);

        if (ready == -1) {
            perror("Error in select");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &readSet)) {
            // User has input
            cin.getline(userInput, sizeof(userInput));

            if (strcmp(userInput, "quit") == 0) {
                break; // Exit the loop
            }

            ssize_t bytesSent = write(clientSocket, userInput, strlen(userInput));
            if (bytesSent == -1) {
                perror("Error sending data to server");
                break;
            }
        }

        if (FD_ISSET(clientSocket, &readSet)) {
            // Server has sent data
            ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer));
            if (bytesRead == -1) {
                perror("Error reading data from server");
                break;
            }

            if (bytesRead == 0) {
                cout << "Server closed the connection." << endl;
                break;
            }

            buffer[bytesRead] = '\0'; // Null-terminate the received data
            cout << buffer;
        }
    }

    // Close the socket
    close(clientSocket);

    return 0;
}
