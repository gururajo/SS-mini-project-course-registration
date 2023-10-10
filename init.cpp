#include <iostream>
#include <fcntl.h>
#include <unistd.h>
using namespace std;
int main()
{
    int fd;
    fd = open("data/details.dat", O_RDWR | O_CREAT, 0600);
    cout << fd;
    int nos = 0;
    write(fd, &nos, sizeof(nos));
    write(fd, &nos, sizeof(nos));
    write(fd, &nos, sizeof(nos));
    truncate("data/students.dat", 0);
    truncate("data/faculty.dat", 0);
    truncate("data/course.dat", 0);

    close(fd);
    return 0;
}