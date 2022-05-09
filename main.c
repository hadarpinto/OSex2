// Hadar Pinto 316460146
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void close_files(int fd1, int fd2) {
    close(fd1);
    close(fd2);
}

int main(int argc, char **argv) {
    int fd1, fd2,b_num1,b_num2;
    char* buf1 = NULL;
    char* buf2 = NULL;


    if (argc != 3) {
        exit(0);
    }
    fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0) {
        exit(0);
    }
    fd2 = open(argv[2], O_RDONLY);
    if (fd2 < 0) {
        close(fd1);
        exit(0);
    }

    b_num1 = read(fd1,&buf1, 1);
    b_num2 = read(fd2, &buf2,1);
    // identical
    while (buf1 == buf2 ) {
        b_num1 = read(fd1,&buf1, 1);
        b_num2 = read(fd2, &buf2,1);
        if (b_num1 == 0 && b_num2 == 0) {
            close_files(fd1,fd2);
            return 1;
        }
    }

    //if this code run - could be only alike 3 or diff 2
    while(1) {
        // different chars that are not space or \n or capital lower case
        if((buf1 != buf2) && (buf1 != ' ') && (buf2 != ' ') && (buf1 != '\n') && (buf2 != '\n') && (buf1+32 != buf2)
            && (buf1-32 != buf2)) {
                close_files(fd1,fd2);
                return 2;
         }
        // space or \n in buf1 - moving on to next byte
        if ((buf1 != buf2) && (buf1 == ' ' || buf1 == '\n')) {
            b_num1 = read(fd1,&buf1, 1);
            continue;
        }
        // space or \n in buf2- moving on to next byte
        if ((buf1 != buf2) && (buf2 == ' ' || buf2 == '\n')) {
            b_num2 = read(fd2, &buf2,1);
            continue;
        }

        // we reached the end of one of the files
        if ((b_num1 == 0 && b_num2 != 0) || (b_num2 == 0 && b_num1 != 0)){
            close_files(fd1,fd2);
            return 2;
        }

        // we reached the end of both files
        if (b_num1 == 0 && b_num2 == 0) {
            close_files(fd1,fd2);
            return 3;
        }

        b_num1 = read(fd1,&buf1, 1);
        b_num2 = read(fd2, &buf2,1);
    }

}
