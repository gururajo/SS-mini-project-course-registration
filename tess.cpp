#include <iostream>
#include <fstream>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <regex>
using namespace std;
int main()
{
    // string s;
    // s= "some string";
    // cout<<s.find(" ")<<endl;
    // cout<<s.substr(4);

    struct some
    {
        char s[5];
        int a;
        bool b;
    };
    some ss, se, *sd;
    // ss = (some *)malloc(sizeof(some));
    // // ss.s = "asdd";
    // strcpy(ss.s, "as");
    // ss.a = 10;
    // ss.b = true;
    // se = ss;
    // sd = new some;
    // ss = *sd;
    // cout << ss.s << endl;
    // cout << se.s << endl;
    // cout << sd->s << endl;
    // cout << ss.a << endl;
    // cout << se.a << endl;
    // ss.s[0] = 'z';
    // cout << ss.s << endl;
    // cout << se.s << endl;
    // cout << sd->s << endl;
    // cout << ss.a << endl;
    // cout << se.a << endl;

    // truncate("shared_file.txt", 0);
    // int fd = open("shared_file.txt", O_RDWR | O_CREAT, 0777);
    // cout << 1 << ss.s << endl;
    // cout << write(fd, &ss, sizeof(ss));
    // // cout << close(fd) << endl;

    // // ss = (some *)malloc(sizeof(struct some));
    // strcpy(ss.s, "bfdsd");
    // ss.a = 31241234;
    // ss.b = false;
    // // ss -> s= "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww";
    // write(fd, &ss, sizeof(ss));
    // lseek(fd, 0, SEEK_SET);
    // // free(ss);
    // cout << sizeof(ss) << endl;
    // // ss = (some *)malloc(sizeof(some));
    // read(fd, &ss, sizeof(ss));
    // cout << ss.s << endl;
    // cout << ss.a << endl;
    // cout << ss.b << endl;

    // // free(ss);
    // // ss = (some *)malloc(sizeof(some));
    // read(fd, &ss, sizeof(ss));
    // cout << ss.s << endl;
    // cout << ss.a << endl;
    // cout << ss.b << endl;

    // regex pat_int("\\d+");
    // char loginid[]= "MTfdgjhgj";
    // string s =  "MTfhsjdfhbks";
    // smatch matches;

    // cout<<regex_search(s,matches, pat_int)<<endl;
    // cout<<matches[0]<<endl;
    char s[] = "12";
    cout << atoi(s);

    return 0;
}