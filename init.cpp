#include<iostream>
#include<fcntl.h>
#include<unistd.h>
using namespace std;
int main(){
    int fd;
    fd = open("data/details.dat",O_RDWR|O_CREAT, 0600);
    cout<<fd;
    int nos = 1024;
    write(fd, &nos, sizeof(nos));
    write(fd, &nos, sizeof(nos));
    write(fd, &nos, sizeof(nos));
    close(fd);
    return 0;
}