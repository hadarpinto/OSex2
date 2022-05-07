// Hadar Pinto 316460146
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


void only_chars(char *str) {
    int count = 0, i;
    for (i = 0; str[i]; i++) {
        if (str[i] != ' ' && str[i] !='\n'){
            str[count++] = str[i];
        }
    }
    str[count] = '\0';
}

void close_files(int fd1, int fd2) {
    close(fd1);
    close(fd2);
}

int main(int argc, char **argv) {
    int fd1, fd2, size;
    char buf1[200], buf2[200];
    if (argc != 2) {
        exit(0);
    }
    fd1 = open(argv[0], O_RDONLY);
    fd2 = open(argv[1], O_RDONLY);
    if (fd1 < 0 || fd2 < 0) {
        exit(0);
    }
    read(fd1,buf1, 200);
    read(fd2, buf2,200);
    if (strcmp(buf1,buf2) == 0) {
        close_files(fd1,fd2);
        return 1;
    }
    else {
        only_chars(buf1);
        only_chars(buf2);
        //check if different
        if (strlen(buf1) != strlen(buf2)) {
            close_files(fd1,fd2);
            return 2;
        }
        // check if alike
        else{
            int i;
            for(i=0;i< strlen(buf1); i++){
                if (buf1[i] != buf2[i] || buf1[i]+32 != buf2[i] || buf1[i]-32 != buf2[i]){
                    return 2;
                }
            }
            return 3;

        }
    }
}
