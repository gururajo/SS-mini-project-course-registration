#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>
using namespace std;

#define BUF_SIZE 1024
#define SMALL_BUF_SIZE 50

int student_count, faculty_count, course_count;
int students_fd = open("data/students.dat", O_RDWR | O_CREAT, 0666);
int faculty_fd = open("data/faculty.dat", O_RDWR | O_CREAT, 0666);
int course_fd = open("data/course.dat", O_RDWR | O_CREAT, 0666);
int detailsfd = open("data/details.dat", O_RDWR);

struct student_struct
{
    char name[SMALL_BUF_SIZE], email[SMALL_BUF_SIZE], username[SMALL_BUF_SIZE], password[SMALL_BUF_SIZE], address[BUF_SIZE];
    char courses_enrolled[6][SMALL_BUF_SIZE];
    int age, courses_enrolled_count;
    bool status;
};

struct faculty_struct
{
    char name[SMALL_BUF_SIZE], department[SMALL_BUF_SIZE], designation[SMALL_BUF_SIZE], email[SMALL_BUF_SIZE], address[BUF_SIZE], username[SMALL_BUF_SIZE], password[SMALL_BUF_SIZE];
    char courses_offered[20][SMALL_BUF_SIZE];
    int courses_offered_count;
};

struct course_struct
{
    char name[SMALL_BUF_SIZE], course_id[SMALL_BUF_SIZE], department[SMALL_BUF_SIZE];
    int no_of_seats, available_seats, credits;
    bool status;
};

void reset_str(char *str, int size)
{
    for (int i = 0; i < size; i++)
    {
        str[i] = '\0';
    }
}

char *substr(char *arr, int begin, int len)
{
    char *res = new char[len + 1];
    for (int i = 0; i < len; i++)
        res[i] = *(arr + begin + i);
    res[len] = 0;
    return res;
}

int validate_student_id(char *username)
{
    int student_index = atoi(substr(username, 2, strlen(username)));
    if (username[0] != 'M' || username[1] != 'T' || student_index >= student_count)
    {
        return -1;
    }
    return student_index;
}

int validate_faculty_id(char *username)
{
    int faculty_index = atoi(substr(username, 2, strlen(username)));
    if (username[0] != 'M' || username[1] != 'T' || faculty_index >= faculty_count)
    {
        return -1;
    }
    return faculty_index;
}

int write_client(int clientfd, const char *data)
{
    int wrote;
    if (wrote = write(clientfd, data, strlen(data)) == -1)
    {
        perror("write");
        return -1;
    }
    return wrote;
}

int read_client(int clientfd, char *data)
{
    int read_bytes;
    if (read_bytes = read(clientfd, data, BUF_SIZE) == -1)
    {
        perror("read");
        return -1;
    }
    return read_bytes;
}

void print_int_array(int *arr, int n)
{
    cout << "-------------------" << endl;
    for (int i = 0; i < n; i++)
    {
        cout << arr[i] << endl;
    }
    cout << "-------------------" << endl;
}

void print_char_array(char **strings, int n)
{
    cout << "-----------------" << endl;
    for (int i = 0; i < n; i++)
        cout << strings[i] << endl;
    cout << "-----------------" << endl;
}

void read_lines_from_file_fd(char **list, int fd, int n)
{
    list = new char *[n];
    char temp[BUF_SIZE];
    for (int i = 0; i < n; i)
    {
        int str_ind = 0;
        char t;
        while (1)
        {
            if (read(fd, &t, sizeof(t)) > 0)
            {
                if (t == '\n')
                {
                    temp[str_ind++] = '\0';
                    list[i] = temp;
                    break;
                }
                else
                {
                    temp[str_ind++] = t;
                }
            }
            else
                break;
        }
    }
    print_char_array(list, n);
}

void load_details()
{
    // if (faculty != NULL)
    // {
    //     for (int i = 0; i < faculty_count; ++i)
    //     {
    //         delete[] faculty[i]; // Free memory for individual C-style strings
    //     }
    //     delete[] faculty;
    // }
    // if (students != NULL)
    // {
    //     for (int i = 0; i < student_count; ++i)
    //     {
    //         delete[] students[i]; // Free memory for individual C-style strings
    //     }
    //     delete[] students;
    // }
    int facultyfd, studentsfd;
    lseek(detailsfd, 0, SEEK_SET);
    read(detailsfd, &student_count, sizeof(student_count));
    read(detailsfd, &faculty_count, sizeof(faculty_count));
    read(detailsfd, &course_count, sizeof(course_count));
}

bool login(int clientfd, char *username, int &user_type)
{
    char revd[BUF_SIZE];
    int read_bytes;
    cout << "Asking username" << endl;
    strcpy(revd, "WELCOME TO IIITB ACADEMIA PORTAL\n\nUsername: \r\n");
    if (write(clientfd, revd, strlen(revd)) == -1)
    {
        perror("write");
        return false;
    }
    reset_str(revd, BUF_SIZE);
    if (read_bytes = read(clientfd, revd, BUF_SIZE) == -1)
    {
        perror("read");
        return false;
    }
    // revd[read_bytes] = '\0';
    strcpy(username, revd);
    cout << read_bytes << " recived: " << revd << endl;
    if (strcmp(revd, "admin") == 0)
    {
        user_type = 1;
        cout << "valid username, asking for password" << endl;
        reset_str(revd, BUF_SIZE);
        strcpy(revd, "Password: \r\n");
        if (write(clientfd, revd, strlen(revd)) == -1)
        {
            perror("write");
            return false;
        }
        cout << "Asking Password: " << endl;
        reset_str(revd, BUF_SIZE);
        if (read(clientfd, revd, BUF_SIZE) == -1)
        {
            perror("read");
            return false;
        }
        cout << "Recived Password: " << endl;
        if (strcmp(revd, "admin") == 0)
        {
            reset_str(revd, BUF_SIZE);
            strcpy(revd, "login success\r\n");
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
        user_type = 2;
        strcpy(revd, "Okaya \r\n");
        if (write(clientfd, revd, strlen(revd)) == -1)
        {
            perror("write");
            return false;
        }
    }
    else if (revd[0] == 'F' && revd[1] == 'C')
    {
        user_type = 3;
        strcpy(revd, "Okaya \r\n");
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

bool cin_student(int clientfd, student_struct &student)
{
    // read data
    char buf[BUF_SIZE];
    if (write_client(clientfd, "Name:\r\n") == -1)
        return false;
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
        return false;
    strcpy(student.name, buf);

    if (write_client(clientfd, "Email:\r\n") == -1)
        return false;
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
        return false;
    strcpy(student.email, buf);

    if (write_client(clientfd, "Age:\r\n") == -1)
        return false;
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
        return false;
    student.age = atoi(buf);

    if (write_client(clientfd, "Address:\r\n") == -1)
        return false;
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
        return false;
    strcpy(student.address, buf);

    for (int i = 0; i < 6; i++)
        reset_str(student.courses_enrolled[i], SMALL_BUF_SIZE);

    student.courses_enrolled_count = 0;

    char username[SMALL_BUF_SIZE];
    // strcpy(usernme, "MT");
    sprintf(username, "MT%d", student_count);
    strcpy(student.username, username);

    student.status = true;

    return true;
}

bool read_student_id(int clientfd, char *buf, int &student_index)
{
    if (write_client(clientfd, "Give the Student_id\r\n") == -1)
        return false;
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
        return false;
    if (student_index = validate_student_id(buf) == -1)
    {
        write_client(clientfd, "WRONG student id\n\r");
        return false;
    }
    return true;
}

bool read_faculty_id(int clientfd, char *buf, int &faculty_index)
{
    if (write_client(clientfd, "Give the Faculty_id\r\n") == -1)
        return false;
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
        return false;
    if (faculty_index = validate_student_id(buf) == -1)
    {
        write_client(clientfd, "WRONG Faculty id\r\n");
        return false;
    }
    return true;
}

void write_student(int clientfd, student_struct &student, int index)
{
    // lseek to end, write data, update details file,
    if (index == -1)
        lseek(students_fd, 0, SEEK_END);
    else
        lseek(students_fd, index * sizeof(student), SEEK_SET);
    write(students_fd, &student, sizeof(student));
    load_details();
    student_count++;
    lseek(detailsfd, 0, SEEK_SET);
    write(detailsfd, &student_count, sizeof(student_count));
}

bool cin_faculty(int clientfd, faculty_struct &faculty)
{
    char buf[BUF_SIZE];
    if (write_client(clientfd, "Name:\r\n") == -1)
        return false;
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
        return false;
    strcpy(faculty.name, buf);

    if (write_client(clientfd, "Department:\r\n") == -1)
        return false;
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
    {
        perror("read");
        return false;
    }
    strcpy(faculty.department, buf);

    if (write_client(clientfd, "Email:\r\n") == -1)
    {
        perror("write");
        return false;
    }
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
    {
        perror("read");
        return false;
    }
    strcpy(faculty.email, buf);

    if (write_client(clientfd, "Designation:\r\n") == -1)
    {
        perror("write");
        return false;
    }
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
    {
        perror("read");
        return false;
    }
    strcpy(faculty.designation, buf);

    if (write_client(clientfd, "Address:\r\n") == -1)
    {
        perror("write");
        return false;
    }
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
    {
        perror("read");
        return false;
    }
    strcpy(faculty.address, buf);

    char username[SMALL_BUF_SIZE];
    // strcpy(usernme, "MT");
    sprintf(username, "FC%d", faculty_count);
    strcpy(faculty.username, username);

    for (int i = 0; i < 20; i++)
    {
        reset_str(faculty.courses_offered[i], SMALL_BUF_SIZE);
    }

    faculty.courses_offered_count = 0;
    return true;
}

void write_faculty(int clientfd, faculty_struct &faculty, int index)
{
    if (index == -1)
        lseek(faculty_fd, 0, SEEK_END);
    else
        lseek(faculty_fd, index * sizeof(faculty), SEEK_SET);
    write(faculty_fd, &faculty, sizeof(faculty));
    load_details();
    faculty_count++;
    lseek(detailsfd, sizeof(int), SEEK_SET);
    write(detailsfd, &faculty_count, sizeof(faculty_count));
}

bool read_record(int filefd, void *add, int index, size_t size)
{
    lseek(filefd, index * size, SEEK_SET);
    if (read(filefd, add, size) == -1)
    {
        perror("read");
        return false;
    }
    return true;
}

void handle_admin(int clientfd, char *username)
{
    char revd[BUF_SIZE], buf[BUF_SIZE];

    while (1)
    {
        load_details();
        string msg = string(".......... Admin Menu ..........\n1. Add Student\n2. "
                            "View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. "
                            "Activate Student\n6. Block Student\n7. Modify Student Details\n8. "
                            "Modify Faculty Details\n9. Logout and Exit\n\nNOTE:only first character will be considered\r\n");
        sprintf(buf, "%d", student_count);
        msg += "Students: " + string(buf);
        sprintf(buf, "%d", faculty_count);
        msg += "\tFaculties: " + string(buf);
        sprintf(buf, "%d", course_count);
        msg += "\tCourses: " + string(buf);
        msg += "\r\n";
        char admin_options[msg.size() + 1];
        strcpy(admin_options, msg.c_str());
        if (write(clientfd, admin_options, strlen(admin_options)) == -1)

        {
            perror("Write");
            return;
        }

        if (read(clientfd, revd, sizeof(revd)) < 1)
        {
            perror("Read");
            return;
        }
        switch (revd[0])
        {
        case '1':
        {
            if (write_client(clientfd, "Adding a Student\r\n") == -1)
            {
                return;
            }
            student_struct student;
            cin_student(clientfd, student);
            write_student(clientfd, student, -1);
            sprintf(buf, "SUCCESSfully added student %s\r\n", student.username);
            if (write_client(clientfd, buf) == -1)
            {
                return;
            }
        }
        break;
        case '2':
        {
            // if (write_client(clientfd, "Give the Student_id\r\n") == -1)
            //     return;
            // reset_str(buf, BUF_SIZE);
            // if (read_client(clientfd, buf) == -1)
            //     return;
            // if (student_index = validate_student_id(buf) == -1)
            // {
            //     write_client(clientfd, "WRONG student id");
            //     break;
            // }
            int student_index;
            if (!read_student_id(clientfd, buf, student_index))
                break;
            student_struct *student_data = (student_struct *)malloc(sizeof(student_struct));
            if (!read_record(students_fd, student_data, student_index, sizeof(student_struct)))
            {
                write_client(clientfd, "ERROR reading file");
                break;
            }
            // string det = "Name: "+ string(student_data->name)+"\n"
            char age[SMALL_BUF_SIZE];
            sprintf(age, "%d", student_data->age);
            write_client(clientfd, string("Name: " + string(student_data->name) + "\nEmail: " + string(student_data->email) + "\nAge: " + string(age) + "\nAddress: " + string(student_data->address) + "\nStatus: " + string((student_data->status) ? "Active\r\n" : "Inactive\r\n")).c_str());
            free(student_data);
            sleep(3);
        }
        break;
        case '3':
        {
            if (write_client(clientfd, "Adding a Faculty\r\n") == -1)
            {
                return;
            }
            faculty_struct faculty;
            cin_faculty(clientfd, faculty);
            write_faculty(clientfd, faculty, -1);
            sprintf(buf, "SUCCESSfully added Faculty %s\r\n", faculty.username);
            if (write_client(clientfd, buf) == -1)
            {
                return;
            }
        }
        break;
        case '4':
        {
            int faculty_index;
            if (!read_faculty_id(clientfd, buf, faculty_index))
                break;

            faculty_struct *faculty_data = (faculty_struct *)malloc(sizeof(faculty_struct));
            if (!read_record(faculty_fd, faculty_data, faculty_index, sizeof(faculty_struct)))
            {
                return;
            }
            // string det = "Name: "+ string(faculty_data->name)+"\n"
            // char age[SMALL_BUF_SIZE];
            // sprintf(age, "%d", faculty_data->age);
            write_client(clientfd, string("Name: " + string(faculty_data->name) + "\nEmail: " + string(faculty_data->email) + "\nDepartment: " + string(faculty_data->department) + "\nAddress: " + string(faculty_data->address) + "Department: " + string(faculty_data->department) + "\r\n").c_str());
            free(faculty_data);
            sleep(3);
        }
        break;
        case '5':
        {
            int student_index;
            if (!read_student_id(clientfd, buf, student_index))
                break;
            student_struct *student_data = (student_struct *)malloc(sizeof(student_struct));
            if (!read_record(students_fd, student_data, student_index, sizeof(student_struct)))
            {
                write_client(clientfd, "ERROR reading file");
                break;
            }
            student_data->status = true;
            student_struct student_data_to_write = *student_data;
            write_student(clientfd, student_data_to_write, student_index);
            // write_record(students_fd, student_data_to_write, student_index, sizeof(student_struct));
            write_client(clientfd, "Student Activated\r\n");
            free(student_data);
            sleep(3);
        }
        break;
        case '6':
            break;
        case '7':
            break;
        case '8':
            break;
        case '9':
            break;
        default:
            break;
        }
    }
}

int handle_client(int clientfd, struct sockaddr_in client_address)
{
    load_details();
    sleep(1);
    char username[BUF_SIZE];
    int user_type;
    if (login(clientfd, username, user_type))
    {
        cout << "Login Success" << username << endl;
        switch (user_type)
        {
        case 1:
            handle_admin(clientfd, username);
            break;

        default:
            break;
        }
    }
    return 0;
}