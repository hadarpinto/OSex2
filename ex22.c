// Hadar Pinto 316460146
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
#define MAX_CHAR 150

struct Students {
    char name[150];
    int grade;
    char comment[50];
};
void read_line_to_arr(char* buf, char* path, int fd);
char* find_c_file(char* studentDir);
int compile_c_file(char* c_file, int fd_error);
int execute_C_file(int fd_input, int fd_error);
int compare_outputs(char correct_output_path[], char main_path[], char student_path[]);
void assign_grade(char *string, struct Students* pointer, int indication);
void write_to_CSV(const struct Students *students, int count);
void remove_temp_files();

void check_path(char path[], char path1[], char path2[], char path3[]);

int main(int argc, char **argv) {
    if (argc !=2) {
        exit(1);
    }

    char path_line1[MAX_CHAR], path_line2[MAX_CHAR], path_line3[MAX_CHAR], *byte_buf = NULL,main_path[MAX_CHAR],
        curr_path[MAX_CHAR];
    int fd_cfg, students_num=0, fd_input;
    getcwd(main_path,MAX_CHAR);
    strcat(main_path,"/");
    fd_cfg = open(argv[1], O_RDONLY);
    if (fd_cfg < 0) {
       return 0;
    }

    read_line_to_arr(byte_buf, path_line1,fd_cfg);
    read_line_to_arr(byte_buf, path_line2,fd_cfg);
    read_line_to_arr(byte_buf, path_line3,fd_cfg);
    check_path(main_path,path_line1,path_line2,path_line3);
    close(fd_cfg);

    fd_input = open(path_line2, O_RDONLY);


    // check number of students' directories
    DIR *pDir;
    struct dirent *pDirent;
    struct stat stat_p;
    if ( (pDir = opendir(path_line1)) == NULL) {
        exit(1);
    }
    while ( (pDirent = readdir(pDir)) != NULL) {
        stat(pDirent->d_name, &stat_p);
        if ((pDirent->d_type == 4) && strcmp(pDirent->d_name, ".") && strcmp(pDirent->d_name, "..")
            && (strcmp(pDirent->d_name,".DS_Store")) ){
            students_num++;
        }
        close(pDir);
    }


    char *student[students_num];
    struct Students students[students_num];
    int i = 0;

    // assign name of directories to char* student[]
    if ( (pDir = opendir(path_line1)) == NULL) {
        exit(1);
    }
    while ( (pDirent = readdir(pDir)) != NULL) {
        stat(pDirent->d_name, &stat_p);
        if ((pDirent->d_type == 4) && strcmp(pDirent->d_name, ".") && strcmp(pDirent->d_name, "..")
            && (strcmp(pDirent->d_name,".DS_Store")) ) {
            student[i++] = pDirent->d_name;
        }
        close(pDir);
    }

    int fd_error = open("errors.txt", O_CREAT | O_WRONLY, 0777);
    //for each student, get inside his dir, compile his .c, input to his a.out, write its output, compare to our, give grade
    chdir(path_line1);

    char check_path[150];
    getcwd(check_path,150);

    int j, pid;
    char* c_file;

    for (j = 0; j < students_num; j++) {
        struct Students* p = &students[j];
        c_file = find_c_file(student[j]);
        if (c_file == -1){
            continue;
        }
        //get inside directory of c file
        chdir(student[j]);

        getcwd(check_path,150);


        //compile gcc file
        if (c_file == NULL) {
            chdir("..");
            assign_grade(student[j], p, 0);
            continue;
        }

        int compile_val = compile_c_file(c_file, fd_error);
        if ( compile_val == -1) {
            assign_grade(student[j], p, -1);
            chdir("..");
            continue;
        }

        getcwd(curr_path,MAX_CHAR);
        int value_execute = execute_C_file(fd_input, fd_error);
        int grade_indication = compare_outputs(path_line3,main_path, curr_path);
        assign_grade(student[j],p,grade_indication);
        chdir("..");

    }

    chdir(main_path);
    getcwd(check_path,150);
    write_to_CSV(students, students_num);
    close(fd_error);
    close(fd_input);

    return 0;
}
int isFile(char* path) {
    struct stat stat_p;
    if (stat(path, &stat_p) != 0){
        return 0;
    }
    return S_ISREG(stat_p.st_mode);
}
int isDirectory(char* path) {
    struct stat stat_p;
    if (stat(path, &stat_p) != 0){
        return 0;
    }
    return S_ISDIR(stat_p.st_mode);
}
void check_path(char main_path[150], char* path1, char* path2, char* path3) {
    char tmp[MAX_CHAR];
    strcpy(tmp, main_path);
     if (path1[0] != '/') {
         strcat(tmp,"/");
         strcat(tmp,path1);
         strcpy(path1,tmp);
     }
    strcpy(tmp, main_path);
     if (path2[0] != '/') {
         strcat(tmp,"/");
         strcat(tmp,path2);
         strcpy(path2,tmp);
     }
    strcpy(tmp, main_path);
    if (path3[0] != '/') {
        strcat(tmp,"/");
        strcat(tmp,path3);
        strcpy(path3,tmp);
    }
    if(!isDirectory(path1)) {
        write(STDOUT_FILENO,"Not a valid directory",21);
        exit(0);
    }
    if(!isFile(path2)){
        write(STDOUT_FILENO,"Input file not exist",20);
        exit(0);
    }
    if(!isFile(path3)){
        write(STDOUT_FILENO,"Output file not exist",20);
        exit(0);
    }


}

void remove_temp_files() {
    remove("output.txt");
    remove("a.out");
}
void write_to_CSV(const struct Students *students, int count) {
    int fd = open("results.csv",O_CREAT | O_WRONLY | O_RDONLY, 0666);
    int i;
    for( i = 0; i < count; i++) {
        char line[MAX_CHAR];
        sprintf(line,  "%s,%d,%s", students[i].name,students[i].grade,students[i].comment);
        write(fd,line,strlen(line));
    }
    close(fd);
}

void assign_grade(char *string, struct Students* pointer, int indication) {
    remove_temp_files();
    strcpy(pointer->name, string);
    if (indication == 1 ){
        pointer->grade = 100;
        strcpy(pointer->comment,"EXCELLENT\n");
    }
    else if(indication == 2) {
        pointer->grade = 50;
        strcpy(pointer->comment,"WRONG\n");
    }
    else if (indication == 3) {
        pointer->grade = 75;
        strcpy(pointer->comment,"SIMILAR\n");
    }
    else if (indication == 0) {
        pointer->grade = 0;
        strcpy(pointer->comment,"NO_C_FILE\n");
    }
    else if (indication == -1){
        pointer->grade = 10;
        strcpy(pointer->comment,"COMPILATION_ERROR\n");
    }
}

int compare_outputs(char correct_output_path[], char main_path[], char student_path[]) {
    chdir(main_path);
    char output_path[MAX_CHAR];
    int pid;
    strcpy(output_path, student_path);
    strcat(output_path,"/output.txt");

    char *compareArg[4] = {"./comp.out", correct_output_path, output_path, NULL};
    pid = fork();
    if (pid < 0){
        exit(-1);
    }
    else if (pid == 0) {
        execvp(compareArg[0],compareArg);
        return -1;
    }
    else {
        int status;
        wait(&status);
        chdir(student_path);
        if(WIFEXITED(status)){
            return WEXITSTATUS(status);
        }
    }

    return 0;
}

void read_line_to_arr(char* buf, char* path, int fd) {
    int i=0, byte;
    byte = read(fd,&buf,1);
    while (buf != '\n' || byte == -1) {
        path[i++] = buf;
        byte = read(fd,&buf,1);
    }
    path[i] = '\0';
    if (byte == -1) {
        write(STDOUT_FILENO,"Error in: READ", 14);
    }
}
char* find_c_file(char* studentDir) {
    DIR *pDir;
    struct dirent *pDirent;
    struct stat stat_p;
    if ( (pDir = opendir(studentDir)) == NULL) {
        return -1;
    }

    // finding c file
    while ( (pDirent = readdir(pDir)) != NULL) {
        stat(pDirent->d_name, &stat_p);
        if ((pDirent->d_name[strlen(pDirent->d_name) -2] == '.')
            && (pDirent->d_name[strlen(pDirent->d_name) -1] == 'c')){
            return pDirent->d_name;
        }
        close(pDir);
    }
    return NULL;
}
int compile_c_file(char* c_file,int fd_error) {
    int pid;
    char* compileArg[3] = {"gcc", c_file, NULL};
    pid = fork();
    if (pid < 0 ){
        exit(1);
    }
    else if (pid == 0) {
        dup2(fd_error,2);
        execvp(compileArg[0],compileArg);
        //check if failed
    }
    else {
        int status;
        waitpid(pid, &status,0);
        if (WIFEXITED(status)) {
            if(WEXITSTATUS(status) == 1) {
                return -1;
            }
            return 0;
        }

    }
}
int execute_C_file(int fd_input, int fd_error) {
    int pid;
    lseek(fd_input,0,SEEK_SET);
    char* executeArg[2] = {"./a.out", NULL};

    int fd_output = open("output.txt", O_CREAT | O_RDWR  , 0777);

    pid =fork();
    if (pid < 0) {
        exit(1);
    }
    else if (pid == 0) {
        dup2(fd_input, STDIN_FILENO);
        dup2(fd_output, STDOUT_FILENO);
        dup2(fd_error,2);
        execvp(executeArg[0], executeArg);
        return -1;

    }
    else {
        int status;
        close(fd_output);
        if(!waitpid(pid, &status, 0)){
            kill(pid,0);
            return -1;
        }
    }


}