// Hadar Pinto 316460146
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>

// read 3 lines, make results.csv at start

//read the file in argv[1] make sure to save all 3 lines
// 1st line is path to folder with folders , we get inside each folder gcc to C program
// then we run the gcc with the 2nd line in cfg, write the output and comparing it with ex21
// we will make results.csv to save grades for each folders name/
struct students {
    char name[150];
    int grade;
    char comment[10];
};
void read_line_to_arr(char* buf, char* path, int fd);
char* find_c_file(char* studentDir);
void compile_c_file(char* c_file);
int execute_C_file(int fd_input);
int compare_outputs(char correct_output_path[], char main_path[], char student_path[]);

void assign_grade(char *string, struct students* pointer, int indication);

int main(int argc, char **argv) {
    if (argc !=2) {
        exit(1);
    }

    char path_line1[150], path_line2[150], path_line3[150], *byte_buf = NULL,main_path[150], curr_path[150];
    int fd_cfg, students_num=0,fd_result, fd_input, fd_correct_output;
    getcwd(main_path,150);
    fd_cfg = open(argv[1], O_RDONLY);
    if (fd_cfg < 0) {
        exit(0);
    }

    read_line_to_arr(byte_buf, path_line1,fd_cfg);
    read_line_to_arr(byte_buf, path_line2,fd_cfg);
    read_line_to_arr(byte_buf, path_line3,fd_cfg);
    close(fd_cfg);

    //make grades file
    fd_result = open("results.csv",O_WRONLY | O_CREAT  , S_IRUSR);
    fd_input = open(path_line2, O_RDONLY);
    if(fd_result < 0) {
        exit(1);
    }

    // check number of students' directories
    DIR *pDir;
    struct dirent *pDirent;
    struct stat stat_p;
    if ( (pDir = opendir(path_line1)) == NULL) {
        exit(1);
    }
    while ( (pDirent = readdir(pDir)) != NULL) {
        stat(pDirent->d_name, &stat_p);
        if((S_ISDIR(stat_p.st_mode)) && strcmp(pDirent->d_name, ".") && strcmp(pDirent->d_name, "..")){
            students_num++;
        }
        close(pDir);
    }
    close(fd_result);
    char *student[students_num+1];
    struct students students[students_num+1];
    int i = 0;

    // assign name of directories to char* student[]
    if ( (pDir = opendir(path_line1)) == NULL) {
        exit(1);
    }
    while ( (pDirent = readdir(pDir)) != NULL) {

        stat(pDirent->d_name, &stat_p);
        if((S_ISDIR(stat_p.st_mode)) && strcmp(pDirent->d_name, ".") && strcmp(pDirent->d_name, "..")){
            student[i++] = pDirent->d_name;
        }
        close(pDir);
    }
    close(fd_result);

    //for each student, get inside his dir, compile his .c, input to his a.out, write its output, compare to our, give grade
    int s = chdir(path_line1);
    int j, pid;
    char* c_file;
    for (j = 0; j <= students_num; j++) {
        struct students* p = &students[j];
        c_file = find_c_file(student[j]);
        //get inside directory of c file
        int d = chdir(student[j]);
        //compile gcc file
        if (c_file == NULL) {
            chdir("..");
            assign_grade(student[j], p, 0);
            continue;
        }
        compile_c_file(c_file);
        getcwd(curr_path,150);
        int value_execute = execute_C_file(fd_input);
        int grade_indication = compare_outputs(path_line3,main_path, curr_path);
        assign_grade(student[j],p,grade_indication);
        remove("output.txt");
        remove("a.out");

        chdir("..");

    }
    close(fd_input);


    return 1;
}

void assign_grade(char *string, struct students* pointer, int indication) {
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
}

int compare_outputs(char correct_output_path[], char main_path[], char student_path[]) {
    chdir(main_path);
    char output_path[150];
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
    while (buf != '\n') {
        path[i++] = buf;
        byte = read(fd,&buf,1);
    }
    path[i] = '\0';
}
char* find_c_file(char* studentDir) {
    DIR *pDir;
    struct dirent *pDirent;
    struct stat stat_p;
    if ( (pDir = opendir(studentDir)) == NULL) {
        exit(1);
    }

    // finding c file
    while ( (pDirent = readdir(pDir)) != NULL) {
        stat(pDirent->d_name, &stat_p);
        if ((pDirent->d_name[strlen(pDirent->d_name) -2] == '.') && (pDirent->d_name[strlen(pDirent->d_name) -1] == 'c')){
            return pDirent->d_name;
        }
        close(pDir);
    }
    return NULL;
}
void compile_c_file(char* c_file) {
    int pid;
    char* compileArg[3] = {"gcc", c_file, NULL};
    pid = fork();
    if (pid < 0 ){
        exit(1);
    }
    else if (pid == 0) {
        execvp(compileArg[0],compileArg);
        //check if failed
    }
    else {
        wait(&pid);
    }
}
int execute_C_file(int fd_input) {
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
//        if (WIFEXITED(status)) {
//            if (WEXITSTATUS(status)== 1){
//
//            }
//        }
    }


}