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
// --------------#defines-----------------------------------------
#define BUF_SIZE 1024
#define SMALL_BUF_SIZE 50
// ---------------------global variables-------------------------------------------
int student_count, faculty_count, course_count;
int students_fd = open("data/students.dat", O_RDWR | O_CREAT, 0666);
int faculty_fd = open("data/faculty.dat", O_RDWR | O_CREAT, 0666);
int course_fd = open("data/course.dat", O_RDWR | O_CREAT, 0666);
int detailsfd = open("data/details.dat", O_RDWR);
// --------------------structures--------------------------------------------
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
    char name[SMALL_BUF_SIZE], course_id[SMALL_BUF_SIZE], department[SMALL_BUF_SIZE], offered_by[SMALL_BUF_SIZE];
    int no_of_seats, available_seats, credits;
    bool status;
};
// --------------------Function declarations--------------------------------------------
bool read_record(int filefd, void *add, int index, size_t size);

// --------------------Function Definitions--------------------------------

int is_number(char *str)
{
    int i;

    for (i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }

    return 1;
}

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

void tostring_student(student_struct *student, char *ret)
{
    char age[SMALL_BUF_SIZE];
    sprintf(age, "%d", student->age);
    strcpy(ret, string("Name: " + string(student->name) + "\nEmail: " + string(student->email) + "\nAge: " + string(age) + "\nAddress: " + string(student->address) + "\nStatus: " + string((student->status) ? "Active\r\n" : "Inactive\r\n")).c_str());
}

void tostring_faculty(faculty_struct *faculty, char *ret)
{
    strcpy(ret, string("Name: " + string(faculty->name) + "\nEmail: " + string(faculty->email) + "\nDepartment: " + string(faculty->department) + "\nAddress: " + string(faculty->address) + "\nDesignation: " + string(faculty->designation) + "\r\n").c_str());
}

void tostring_course(course_struct *course_data, char *ret)
{

    char no_of_seats[SMALL_BUF_SIZE], credits[SMALL_BUF_SIZE], no_of_available_seats[SMALL_BUF_SIZE];
    sprintf(no_of_seats, "%d", course_data->no_of_seats);
    sprintf(credits, "%d", course_data->credits);
    sprintf(no_of_available_seats, "%d", course_data->available_seats);
    strcpy(ret, string("Name: " + string(course_data->name) + "\nDepartment: " + string(course_data->department) + "\nOffered By: " + string(course_data->offered_by) + "\nTotal Seats: " + string(no_of_seats) + "\nCredits: " + string(credits) + "\nAvailable Seats: " + string(no_of_available_seats) + "\nStatus: " + string((course_data->status) ? "Active" : "Inactive") + "\r\n").c_str());
}

char *indexed_tostring_char_array(char **array, int n)
{
    char *ret_string = (char *)malloc(BUF_SIZE * sizeof(char));
    char temp[BUF_SIZE];
    reset_str(ret_string, BUF_SIZE);
    for (int i = 0; i < n; i++)
    {
        reset_str(temp, BUF_SIZE);
        sprintf(temp, "%d: %s\n", i, array[i]);
        strcat(ret_string, temp);
    }
    return ret_string;
}

char *tostring_char_array(char *array[], int n)
{
    char *ret_string = (char *)malloc(BUF_SIZE * sizeof(char));
    char temp[BUF_SIZE];
    reset_str(ret_string, BUF_SIZE);
    for (int i = 0; i < n; i++)
    {
        reset_str(temp, BUF_SIZE);
        sprintf(temp, "%s\n", array[i]);
        strcat(ret_string, temp);
    }
    return ret_string;
}

int validate_student_id(char *username)
{
    cout << "substr: " << substr(username, 2, strlen(username)) << endl;
    int student_index = atoi(substr(username, 2, strlen(username)));
    // cout << "student_index3: " << student_index << endl;
    if (username[0] != 'M' || username[1] != 'T' || student_index >= student_count)
    {
        return -1;
    }
    return student_index;
}

int validate_faculty_id(char *username)
{
    int faculty_index = atoi(substr(username, 2, strlen(username)));
    if (username[0] != 'F' || username[1] != 'C' || faculty_index >= faculty_count)
    {
        return -1;
    }
    return faculty_index;
}

int validate_course_id(char *course_id)
{
    int course_index = atoi(substr(course_id, 2, strlen(course_id)));
    if (course_id[0] != 'C' || course_id[1] != 'O' || course_index >= course_count)
    {
        return -1;
    }
    return course_index;
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
        char username[strlen(revd) + 1];
        strcpy(username, revd);
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
        int index = validate_student_id(username);
        if (index == -1)
        {
            strcpy(revd, "INVALID USERNAME");
            write(clientfd, revd, strlen(revd));
            return false;
        }
        student_struct *student_data = (student_struct *)malloc(sizeof(student_struct));
        if (!read_record(students_fd, student_data, index, sizeof(student_struct)))
        {
            strcpy(revd, "Some error occurred");
            write(clientfd, revd, strlen(revd));
            return false;
        }
        cout << "read password" << student_data->password << endl;
        if (strcmp(revd, student_data->password) == 0)
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
    else if (revd[0] == 'F' && revd[1] == 'C')
    {
        user_type = 3;
        char username[strlen(revd) + 1];
        strcpy(username, revd);
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
        int index = validate_faculty_id(username);
        if (index == -1)
        {
            strcpy(revd, "INVALID USERNAME");
            write(clientfd, revd, strlen(revd));
            return false;
        }
        faculty_struct *faculty_data = (faculty_struct *)malloc(sizeof(faculty_struct));
        if (!read_record(faculty_fd, faculty_data, index, sizeof(faculty_struct)))
        {
            strcpy(revd, "Some error occurred");
            write(clientfd, revd, strlen(revd));
            return false;
        }
        if (strcmp(revd, faculty_data->password) == 0)
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

    if ((student_index = validate_student_id(buf)) == -1)
    {
        write_client(clientfd, "WRONG student id\n\r");
        return false;
    }
    // cout << "Student index2: " << student_index << endl;
    return true;
}

bool read_faculty_id(int clientfd, char *buf, int &faculty_index)
{
    if (write_client(clientfd, "Give the Faculty_id\r\n") == -1)
        return false;
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
        return false;
    if ((faculty_index = validate_faculty_id(buf)) == -1)
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
    {
        strcpy(student.password, "iamstudent");
        lseek(students_fd, 0, SEEK_END);
    }
    else
        lseek(students_fd, index * sizeof(student), SEEK_SET);
    write(students_fd, &student, sizeof(student));
    load_details();
    if (index == -1)
    {
        student_count++;
        lseek(detailsfd, 0, SEEK_SET);
        write(detailsfd, &student_count, sizeof(student_count));
    }
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
    {
        strcpy(faculty.password, "iamnotstudent");
        lseek(faculty_fd, 0, SEEK_END);
    }
    else
        lseek(faculty_fd, index * sizeof(faculty), SEEK_SET);
    write(faculty_fd, &faculty, sizeof(faculty));
    load_details();
    if (index == -1)
    {
        faculty_count++;
        lseek(detailsfd, sizeof(int), SEEK_SET);
        write(detailsfd, &faculty_count, sizeof(faculty_count));
    }
}

bool cin_course(int clientfd, course_struct &course, char *faculty_id)
{
    char buf[BUF_SIZE];

    if (write_client(clientfd, "Name:\r\n") == -1)
        return false;
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
        return false;
    strcpy(course.name, buf);

    if (write_client(clientfd, "Department:\r\n") == -1)
        return false;
    reset_str(buf, BUF_SIZE);
    if (read_client(clientfd, buf) == -1)
    {
        perror("read");
        return false;
    }
    strcpy(course.department, buf);

    if (write_client(clientfd, "No Of Seats:\r\n") == -1)
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
    if (is_number(buf))
    {
        course.no_of_seats = atoi(buf);
        course.available_seats = course.no_of_seats;
    }

    if (write_client(clientfd, "Credits:\r\n") == -1)
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
    if (is_number(buf))
    {
        course.credits = atoi(buf);
    }

    course.status = true;

    strcpy(course.offered_by, faculty_id);

    char username[SMALL_BUF_SIZE];
    // strcpy(usernme, "MT");
    sprintf(username, "CO%d", course_count);
    strcpy(course.course_id, username);

    return true;
}

void write_course(int clinetfd, course_struct &course, int index)
{
    if (index == -1)
    {
        lseek(course_fd, 0, SEEK_END);
    }
    else
        lseek(course_fd, index * sizeof(course), SEEK_SET);
    write(course_fd, &course, sizeof(course));
    load_details();
    if (index == -1)
    {
        course_count++;
        lseek(detailsfd, 2 * sizeof(int), SEEK_SET);
        write(detailsfd, &course_count, sizeof(course_count));
    }
}

bool read_record(int filefd, void *add, int index, size_t size)
{
    cout << "index: " << index << " size: " << size << endl;
    cout << "lseek: " << lseek(filefd, index * size, SEEK_SET) << endl;
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
            cout << "student_index: " << student_index << endl;
            student_struct *student_data = (student_struct *)malloc(sizeof(student_struct));
            if (!read_record(students_fd, student_data, student_index, sizeof(student_struct)))
            {
                write_client(clientfd, "ERROR reading file");
                break;
            }
            // string det = "Name: "+ string(student_data->name)+"\n"

            tostring_student(student_data, buf);
            write_client(clientfd, buf);
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
            tostring_faculty(faculty_data, buf);
            write_client(clientfd, buf);
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
            tostring_student(student_data, buf);
            write_client(clientfd, buf);
            free(student_data);
            sleep(3);
        }
        break;
        case '6':
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
            student_data->status = false;
            student_struct student_data_to_write = *student_data;
            write_student(clientfd, student_data_to_write, student_index);
            // write_record(students_fd, student_data_to_write, student_index, sizeof(student_struct));
            write_client(clientfd, "Student Activated\r\n");
            tostring_student(student_data, buf);
            write_client(clientfd, buf);
            free(student_data);
            sleep(3);
        }
        break;
        case '7':
        {
            if (write_client(clientfd, "UPDATE Student\r\n") == -1)
            {
                break;
            }
            int student_index;
            if (!read_student_id(clientfd, buf, student_index))
                break;
            student_struct *student_data = (student_struct *)malloc(sizeof(student_struct));
            if (!read_record(students_fd, student_data, student_index, sizeof(student_struct)))
            {
                write_client(clientfd, "ERROR reading file");
                break;
            }
            // student_data->status = true;
            student_struct student_data_to_write = *student_data;
            tostring_student(student_data, buf);
            write_client(clientfd, buf);
            cin_student(clientfd, student_data_to_write);
            write_student(clientfd, student_data_to_write, student_index);
            // write_record(students_fd, student_data_to_write, student_index, sizeof(student_struct));
            write_client(clientfd, "Student Updated\r\n");
            free(student_data);
            sleep(3);
        }
        break;
        case '8':
        {
            if (write_client(clientfd, "UPDATE faculty\r\n") == -1)
            {
                break;
            }
            int faculty_index;
            if (!read_faculty_id(clientfd, buf, faculty_index))
                break;
            faculty_struct *faculty_data = (faculty_struct *)malloc(sizeof(faculty_struct));
            if (!read_record(faculty_fd, faculty_data, faculty_index, sizeof(faculty_struct)))
            {
                write_client(clientfd, "ERROR reading file");
                break;
            }
            // faculty_data->status = true;
            faculty_struct faculty_data_to_write = *faculty_data;
            tostring_faculty(faculty_data, buf);
            write_client(clientfd, buf);
            cin_faculty(clientfd, faculty_data_to_write);
            write_faculty(clientfd, faculty_data_to_write, faculty_index);
            // write_record(students_fd, student_data_to_write, student_index, sizeof(faculty_struct));
            write_client(clientfd, "Faculty Updated\r\n");
            free(faculty_data);
            sleep(3);
        }
        break;
        case '9':
            // break;
            return;
        default:
            break;
        }
    }
}

void handle_student(int clientfd, char *username)
{
    char revd[BUF_SIZE], buf[BUF_SIZE];
    student_struct student_data_main;
    int student_index;
    student_index = validate_student_id(username);
    read_record(students_fd, &student_data_main, student_index, sizeof(student_struct));

    while (1)
    {
        load_details();

        char msg[2 * BUF_SIZE];
        sprintf(msg, ".......... Welcome to Student Menu %s  ..........\n1. View All Courses\n2. Enroll New Course\n3. Drop Course\n4. View Enrolled Course Details\n5. Change Password\n6. Logout and Exit\r\nStudents: %d\tFaculties: %d\tCourses: %d\r\n", student_data_main.name, student_count, faculty_count, course_count);

        if (write(clientfd, msg, strlen(msg)) == -1)

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
            // char courses_list[course_count][BUF_SIZE];
            char **courses_list;
            courses_list = (char **)malloc(course_count * sizeof(char *));
            for (int i = 0; i < course_count; i++)
            {
                courses_list[i] = (char *)malloc(BUF_SIZE);
            }

            course_struct course;
            int active_courses = 0;
            sprintf(courses_list[active_courses++], "%s: %s\n", "Option: ", "Course Name");
            for (int i = 0; i < course_count; i++)
            {
                read_record(course_fd, &course, i, sizeof(course_struct));
                if (course.status)
                    sprintf(courses_list[active_courses++], "%d: %s", i, course.name);
            }
            sprintf(courses_list[active_courses++], "%s\n", "Give the Option");
            char *course_list_string;
            course_list_string = tostring_char_array(courses_list, active_courses);
            write_client(clientfd, course_list_string);
            free(course_list_string);
            reset_str(buf, BUF_SIZE);
            read_client(clientfd, buf);
            if (!is_number(buf))
            {
                write_client(clientfd, "Invalid Input\r\n");
                break;
            }
            int course_index = atoi(buf);
            if (course_index >= course_count)
            {
                write_client(clientfd, "Not a valid Course\r\n");
                break;
            }
            course_struct course_data;
            if (!read_record(course_fd, &course_data, course_index, sizeof(course_struct)))
            {
                cout << "Error reading course file" << endl;
                break;
            }
            tostring_course(&course_data, buf);
            if (write_client(clientfd, buf) == -1)
            {
                break;
            }
            sleep(3);
        }
        break;
        case '2':
        {
            // char courses_list[course_count][BUF_SIZE];
            char **courses_list;
            courses_list = (char **)malloc(course_count * sizeof(char *));
            for (int i = 0; i < course_count; i++)
            {
                courses_list[i] = (char *)malloc(BUF_SIZE);
            }

            course_struct course;
            int active_courses = 0;
            sprintf(courses_list[active_courses++], "%s: %s\n", "Option: ", "Course Name");
            for (int i = 0; i < course_count; i++)
            {
                read_record(course_fd, &course, i, sizeof(course_struct));
                if (course.status)
                    sprintf(courses_list[active_courses++], "%d: %s", i, course.name);
            }
            sprintf(courses_list[active_courses++], "%s\n", "Give the Option");
            char *course_list_string;
            course_list_string = tostring_char_array(courses_list, active_courses);
            write_client(clientfd, course_list_string);
            free(course_list_string);
            reset_str(buf, BUF_SIZE);
            read_client(clientfd, buf);
            if (!is_number(buf))
            {
                write_client(clientfd, "Invalid Input\r\n");
                break;
            }
            int course_index = atoi(buf);
            if (course_index >= course_count)
            {
                write_client(clientfd, "Not a valid Course\r\n");
                break;
            }
            course_struct course_data;
            if (!read_record(course_fd, &course_data, course_index, sizeof(course_struct)))
            {
                cout << "Error reading course file" << endl;
                break;
            }
            tostring_course(&course_data, buf);
            if (write_client(clientfd, buf) == -1)
            {
                break;
            }
            sleep(3);
            cout << "availlble seats: " << course_data.available_seats << endl;
            course_data.available_seats--;
            write_course(clientfd, course_data, course_index);
            strcpy(student_data_main.courses_enrolled[student_data_main.courses_enrolled_count++], course_data.course_id);
            write_student(clientfd, student_data_main, student_index);
            write_client(clientfd, "Added the Course\r\n");
        }
        break;
        case '3':
        {
            char *courses_optted_string;
            char **courses_optted;
            courses_optted = (char **)malloc(student_data_main.courses_enrolled_count * sizeof(char *));
            for (int i = 0; i < student_data_main.courses_enrolled_count; i++)
            {
                courses_optted[i] = (char *)malloc(BUF_SIZE);
                strcpy(courses_optted[i], student_data_main.courses_enrolled[i]);
            }
            courses_optted_string = indexed_tostring_char_array(courses_optted, student_data_main.courses_enrolled_count);
            write_client(clientfd, courses_optted_string);
            reset_str(buf, BUF_SIZE);
            read_client(clientfd, buf);
            if (!is_number(buf))
            {
                write_client(clientfd, "Invalid Input\r\n");
                break;
            }
            int option = atoi(buf);
            char course_id[SMALL_BUF_SIZE];
            strcpy(course_id, student_data_main.courses_enrolled[option]);
            for (int i = option; i < student_data_main.courses_enrolled_count - 1; i++)
            {
                strcpy(student_data_main.courses_enrolled[i], student_data_main.courses_enrolled[i + 1]);
            }
            student_data_main.courses_enrolled_count--;
            write_student(clientfd, student_data_main, student_index);
            course_struct course_data;
            int course_index = validate_course_id(course_id);
            if (!read_record(course_fd, &course_data, course_index, sizeof(course_struct)))
            {
                cout << "Error reading course file" << endl;
                break;
            }
            course_data.available_seats++;
            write_course(clientfd, course_data, course_index);
            write_client(clientfd, "Dropped the Course\r\n");
        }
        break;
        case '4':
        {
            char *courses_optted_string;
            char **courses_optted;
            courses_optted = (char **)malloc(student_data_main.courses_enrolled_count * sizeof(char *));
            for (int i = 0; i < student_data_main.courses_enrolled_count; i++)
            {
                courses_optted[i] = (char *)malloc(BUF_SIZE);
                strcpy(courses_optted[i], student_data_main.courses_enrolled[i]);
            }
            courses_optted_string = indexed_tostring_char_array(courses_optted, student_data_main.courses_enrolled_count);
            write_client(clientfd, courses_optted_string);
            reset_str(buf, BUF_SIZE);
            read_client(clientfd, buf);
            if (!is_number(buf))
            {
                write_client(clientfd, "Invalid Input\r\n");
                break;
            }
            int option = atoi(buf);
            int course_index = validate_course_id(student_data_main.courses_enrolled[option]);
            course_struct course_data;
            if (!read_record(course_fd, &course_data, course_index, sizeof(course_struct)))
            {
                cout << "ERROR" << endl;
                break;
            }
            tostring_course(&course_data, buf);
            if (write_client(clientfd, buf) == -1)
            {
                break;
            }
            sleep(3);
        }
        break;
        case '5':
        {
            reset_str(buf, BUF_SIZE);
            write_client(clientfd, "Give the Password (not gonna ask again)\r\n");
            read_client(clientfd, buf);
            strcpy(student_data_main.password, buf);

            write_student(clientfd, student_data_main, student_index);
            // write_record(students_fd, student_data_to_write, student_index, sizeof(faculty_struct));
            write_client(clientfd, "Password UPdated\r\n");
            tostring_student(&student_data_main, buf);
            write_client(clientfd, buf);
            sleep(3);
        }
        break;
        case '6':
            return;
        default:
            break;
        }
    }
}

void handle_faculty(int clientfd, char *username)
{
    char revd[BUF_SIZE], buf[BUF_SIZE];
    faculty_struct faculty_data_main;
    int faculty_index;
    faculty_index = validate_faculty_id(username);
    read_record(faculty_fd, &faculty_data_main, faculty_index, sizeof(faculty_struct));

    while (1)
    {
        load_details();

        char msg[2 * BUF_SIZE];
        sprintf(msg, ".......... Welcome to Faculty Menu %s  ..........\n1. View Offering Courses\n2. Add New Course\n3. Activate/Inactivate Course\n4. Update Course Details\n5. Change Password\n6. Logout and Exit\r\nStudents: %d\tFaculties: %d\tCourses: %d\r\n", faculty_data_main.name, student_count, faculty_count, course_count);

        if (write(clientfd, msg, strlen(msg)) == -1)

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
            char courses_string[BUF_SIZE];
            char temp[BUF_SIZE];
            reset_str(courses_string, BUF_SIZE);
            for (int i = 0; i < faculty_data_main.courses_offered_count; i++)
            {
                reset_str(temp, BUF_SIZE);
                sprintf(temp, "%d: %s\n", i, faculty_data_main.courses_offered[i]);
                strcat(courses_string, temp);
            }
            if (write_client(clientfd, courses_string) == -1)
            {
                break;
            }
            int course_index_of_course;
            reset_str(buf, BUF_SIZE);
            if (read_client(clientfd, buf) == -1)
            {
                break;
            }
            if (!is_number(buf))
            {
                write_client(clientfd, "WRONG option\r\n");
                break;
            }
            course_index_of_course = atoi(buf);
            int course_index;
            course_index = validate_course_id(faculty_data_main.courses_offered[course_index_of_course]);
            if (course_index < 0)
            {
                cout << "Course error" << endl;
                break;
            }
            course_struct course_data;
            if (!read_record(course_fd, &course_data, course_index, sizeof(course_struct)))
            {
                cout << "Error reading course file" << endl;
                break;
            }
            tostring_course(&course_data, buf);
            if (write_client(clientfd, buf) == -1)
            {
                break;
            }
            sleep(3);
        }

        break;
        case '2':
        {
            if (faculty_data_main.courses_offered_count >= 20)
            {
                if (write_client(clientfd, "Cannot Add any more courses(Max 20)\r\n") == -1)
                {
                    return;
                }
            }
            if (write_client(clientfd, "Adding a new Course\r\n") == -1)
            {
                return;
            }
            course_struct course;
            cin_course(clientfd, course, username);
            write_course(clientfd, course, -1);

            faculty_struct *faculty_data = (faculty_struct *)malloc(sizeof(faculty_struct));
            if (!read_record(faculty_fd, faculty_data, faculty_index, sizeof(faculty_struct)))
            {
                write_client(clientfd, "ERROR reading file");
                break;
            }
            // faculty_data->status = true;
            faculty_struct faculty_data_to_write = *faculty_data;
            strcpy(faculty_data_to_write.courses_offered[faculty_data_to_write.courses_offered_count], course.course_id);
            faculty_data_to_write.courses_offered_count++;
            write_faculty(clientfd, faculty_data_to_write, faculty_index);
            faculty_data_main = faculty_data_to_write;

            // write_record(students_fd, student_data_to_write, student_index, sizeof(faculty_struct));
            free(faculty_data);

            sprintf(buf, "SUCCESSfully added course %s\r\n", course.course_id);
            if (write_client(clientfd, buf) == -1)
            {
                return;
            }
            sleep(3);
        }
        break;
        case '3':
        {
            char courses_string[BUF_SIZE];
            char temp[BUF_SIZE];
            reset_str(courses_string, BUF_SIZE);
            for (int i = 0; i < faculty_data_main.courses_offered_count; i++)
            {
                reset_str(temp, BUF_SIZE);
                sprintf(temp, "%d: %s\n", i, faculty_data_main.courses_offered[i]);
                strcat(courses_string, temp);
            }
            if (write_client(clientfd, courses_string) == -1)
            {
                break;
            }
            int course_index_of_course;
            reset_str(buf, BUF_SIZE);
            if (read_client(clientfd, buf) == -1)
            {
                break;
            }
            if (!is_number(buf))
            {
                write_client(clientfd, "WRONG option\r\n");
                break;
            }
            course_index_of_course = atoi(buf);
            int course_index;
            course_index = validate_course_id(faculty_data_main.courses_offered[course_index_of_course]);
            if (course_index < 0)
            {
                cout << "Course error" << endl;
                break;
            }
            course_struct course_data;
            if (!read_record(course_fd, &course_data, course_index, sizeof(course_struct)))
            {
                cout << "Error reading course file" << endl;
                break;
            }
            course_data.status = !course_data.status;
            write_course(clientfd, course_data, course_index);
            reset_str(buf, BUF_SIZE);
            tostring_course(&course_data, buf);
            write_client(clientfd, buf);
            if (write_client(clientfd, "Updated Successfully\r\n") == -1)
            {
                cout << "client write error" << endl;
                break;
            }
            sleep(3);
        }
        break;
        case '4':
        {
            char courses_string[BUF_SIZE];
            char temp[BUF_SIZE];
            reset_str(courses_string, BUF_SIZE);
            for (int i = 0; i < faculty_data_main.courses_offered_count; i++)
            {
                reset_str(temp, BUF_SIZE);
                sprintf(temp, "%d: %s\n", i, faculty_data_main.courses_offered[i]);
                strcat(courses_string, temp);
            }
            if (write_client(clientfd, courses_string) == -1)
            {
                break;
            }
            int course_index_of_course;
            reset_str(buf, BUF_SIZE);
            if (read_client(clientfd, buf) == -1)
            {
                break;
            }
            if (!is_number(buf))
            {
                write_client(clientfd, "WRONG option\r\n");
                break;
            }
            course_index_of_course = atoi(buf);
            int course_index;
            course_index = validate_course_id(faculty_data_main.courses_offered[course_index_of_course]);
            if (course_index < 0)
            {
                cout << "Course error" << endl;
                break;
            }
            course_struct course_data;
            if (!read_record(course_fd, &course_data, course_index, sizeof(course_struct)))
            {
                cout << "Error reading course file" << endl;
                break;
            }
            bool course_status_bk = course_data.status;
            int available_seats = course_data.available_seats;
            char temp_course_id[SMALL_BUF_SIZE];
            strcpy(temp_course_id, course_data.course_id);
            cin_course(clientfd, course_data, faculty_data_main.username);
            course_data.status = course_status_bk;
            course_data.available_seats = available_seats;
            strcpy(course_data.course_id, temp_course_id);
            cout << "course index: " << course_index << endl;
            write_course(clientfd, course_data, course_index);
            if (write_client(clientfd, "Updated Successfully\r\n") == -1)
            {
                cout << "client write error" << endl;
                break;
            }
            sleep(3);
        }
        break;
        case '5':
        {
            reset_str(buf, BUF_SIZE);
            write_client(clientfd, "Give the Password (not gonna ask again)\r\n");
            read_client(clientfd, buf);
            strcpy(faculty_data_main.password, buf);

            write_faculty(clientfd, faculty_data_main, faculty_index);
            // write_record(students_fd, student_data_to_write, faculty_index, sizeof(faculty_struct));
            write_client(clientfd, "Password UPdated\r\n");
            tostring_faculty(&faculty_data_main, buf);
            write_client(clientfd, buf);
            sleep(3);
        }
        break;
        case '6':
            return;
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
        case 2:
            handle_student(clientfd, username);
            break;
        case 3:
            handle_faculty(clientfd, username);
        default:

            break;
        }
    }
    return 0;
}