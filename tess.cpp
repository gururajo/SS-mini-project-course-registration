#include <iostream>
#include <fstream>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;
void* performFileOperation(void* arg) {
    const char* filename = "shared_file.txt";
    const char* threadName = static_cast<const char*>(arg);

    // Open the file
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        return nullptr;
    }

    // Create a file lock
    struct flock lock;
    lock.l_type = F_WRLCK;  // Write lock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;  // Lock the entire file

    // Lock the file
    cout<<"locking the file"<<endl;
    if (fcntl(fd, F_SETLKW, &lock) == -1) {
        perror("Error locking file");
        close(fd);
        return nullptr;
    }
    cout<<"Got the lock"<<endl;


    // Critical section: Perform file operations
    std::cout << threadName << " locked the file and is now writing to it." << std::endl;
    sleep(5);
    std::ofstream fileStream;
    fileStream.open(filename, std::ios::app);
    if (fileStream.is_open()) {
        fileStream << threadName << " wrote to the file." << std::endl;
        fileStream.close();
    }

    // Unlock the file
    lock.l_type = F_UNLCK;  // Unlock
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("Error unlocking file");
    }
    cout<<threadName<<"Unlocked the file"<<endl;
    close(fd);
    return nullptr;
}

int main() {
    pthread_t thread1, thread2;
    const char* threadName1 = "Thread 1";
    const char* threadName2 = "Thread 2";

    // Create two threads
    pthread_create(&thread1, nullptr, performFileOperation, (void*)threadName1);
    pthread_create(&thread2, nullptr, performFileOperation, (void*)threadName2);

    // Wait for the threads to finish
    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);

    return 0;
}
