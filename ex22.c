// Hadar Pinto 316460146
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

// read 3 lines, make results.csv at start

//read the file in argv[1] make sure to save all 3 lines
// 1st line is path to folder with folders , we get inside each folder gcc to C program
// then we run the gcc with the 2nd line in cfg, write the output and comparing it with ex21
// we will make results.csv to save grades for each folders name/
void read_line_to_arr(char* buf, char* path, int fd) {
    int i=0, byte;
    byte = read(fd,&buf,1);
    while (buf != '\n') {
        path[i++] = buf;
        byte = read(fd,&buf,1);
    }
    path[i] = '\0';
}

int main(int argc, char **argv) {
    if (argc !=2) {
        exit(1);
    }

    char path_line1[150], path_line2[150], path_line3[150], *byte_buf = NULL;
    int fd_cfg, students_num=0,fd_result;

    fd_cfg = open(argv[1], O_RDONLY);
    if (fd_cfg < 0) {
        exit(0);
    }

    read_line_to_arr(byte_buf, path_line1,fd_cfg);
    read_line_to_arr(byte_buf, path_line2,fd_cfg);
    read_line_to_arr(byte_buf, path_line3,fd_cfg);
    close(fd_cfg);

    //make grades file
    fd_result = open("/home/hadar/Public/OperationSystem/ex2/results.csv",O_WRONLY | O_CREAT  , S_IRUSR);
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
        printf("%s\n",pDirent->d_name);
        stat(pDirent->d_name, &stat_p);
        if((S_ISDIR(stat_p.st_mode)) && strcmp(pDirent->d_name, ".") && strcmp(pDirent->d_name, "..")){
            students_num++;
        }
        close(pDir);
    }
    close(fd_result);
    char *student[students_num+1];
    int i = 0;

    // assign name of directories to char* student[]
    if ( (pDir = opendir(path_line1)) == NULL) {
        exit(1);
    }
    while ( (pDirent = readdir(pDir)) != NULL) {

        stat(pDirent->d_name, &stat_p);
        if((S_ISDIR(stat_p.st_mode)) && strcmp(pDirent->d_name, ".") && strcmp(pDirent->d_name, "..")){
            student[i++] =pDirent->d_name;
        }
        close(pDir);
    }
    close(fd_result);

    //for each student, get inside his dir, gcc .c , input to his a.out, write its output, compare to our , give grade
    int s = chdir(path_line1);
    int j;
    for (j = 0; j <= students_num; j++) {
        int f= chdir(student[j]);










        chdir("..");
    }










    return 1;
}