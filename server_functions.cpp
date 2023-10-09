#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
using namespace std;

#define BUF_SIZE 1024

int student_count, faculty_count, course_count;
char **students, **faculty;

void reset_str(char *str)
{
    for (int i = 0; i < sizeof(str); i++)
    {
        str[i] = '\0';
    }
}

bool login(int clientfd)
{
    char revd[BUF_SIZE];
    int read_bytes;
    cout << "Asking username" << endl;
    strcpy(revd, "Username: \r");
    if (write(clientfd, revd, strlen(revd)) == -1)
    {
        perror("write");
        return false;
    }
    reset_str(revd);
    if (read_bytes = read(clientfd, revd, BUF_SIZE) == -1)
    {
        perror("read");
        return false;
    }
    // revd[read_bytes] = '\0';
    cout << read_bytes << "recived: " << revd << endl;
    if (strcmp(revd, "admin") == 0)
    {
        cout << "valid username, asking for password" << endl;
        reset_str(revd);
        strcpy(revd, "Password: \r");
        if (write(clientfd, revd, strlen(revd)) == -1)
        {
            perror("write");
            return false;
        }
        cout << "Asking Password: " << endl;
        reset_str(revd);
        if (read(clientfd, revd, BUF_SIZE) == -1)
        {
            perror("read");
            return false;
        }
        cout << "Recived Password: " << endl;
        if (strcmp(revd, "admin") == 0)
        {
            reset_str(revd);
            strcpy(revd, "login success");
            if (write(clientfd, revd, strlen(revd)) == -1)
            {
                perror("write");
                return false;
            }
            return true;
        }
    }
    else if (revd[0] == 'M' && revd[1] == 'T')
    {
        strcpy(revd, "Okaya \r");
        if (write(clientfd, revd, strlen(revd)) == -1)
        {
            perror("write");
            return false;
        }
    }
    else if (revd[0] == 'F' && revd[1] == 'C')
    {
        strcpy(revd, "Okaya \r");
        if (write(clientfd, revd, strlen(revd)) == -1)
        {
            perror("write");
            return false;
        }
    }
    else
    {
        strcpy(revd, "WRONG USERNAME");
        write(clientfd, revd, strlen(revd));
        return false;
    }

    return false;
}
int handle_client(int clientfd, struct sockaddr_in client_address)
{
    load_details();
    sleep(1);
    if (login(clientfd))
    {
        cout << "Login Success" << endl;
    }
    return 0;
}