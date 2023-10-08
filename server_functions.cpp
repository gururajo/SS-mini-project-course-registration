#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include<iostream>
using   namespace std;

#define BUF_SIZE 1024


bool login(int clientfd){
    char revd[BUF_SIZE];
    int read_bytes;
    strcpy(revd, "Username: ");
    if(write(clientfd, revd, strlen(revd))==-1){
        perror("write");
        return false;
    }
    if(read_bytes = read(clientfd, revd,BUF_SIZE)==-1){
        perror("read");
        return false;
    }
    // revd[read_bytes] = '\0';
    cout<<read_bytes<<"recived: "   << revd<<endl;
    if(strcmp(revd, "admin")==0){
        strcpy(revd,"Password: ");
        if(write(clientfd,revd, strlen(revd))==-1){ 
            perror("write");
            return false;
        }
        if(read(clientfd, revd, BUF_SIZE)==-1){
            perror("read");
            return false;
        }
        if(strcmp(revd, "admin")==0){
            return true;
        }
    }
    else if(revd[0] == 'M' && revd[1] == 'T'){
        strcpy(revd,"Okaya ");
        if(write(clientfd,revd, strlen(revd))==-1){ 
            perror("write");
            return false;
        }
    }
    else if(revd[0] == 'F' && revd[1] == 'C'){
        strcpy(revd,"Okaya ");
        if(write(clientfd,revd, strlen(revd))==-1){ 
            perror("write");
            return false;
        }
    }
    else{
        strcpy(revd,"WRONG USERNAME");
        write(clientfd, revd, strlen(revd));
        return false;
    }


    return false;
}
int handle_client(int clientfd, struct sockaddr_in client_address) {
    if(login(clientfd)) {

    }
    return 0;
}