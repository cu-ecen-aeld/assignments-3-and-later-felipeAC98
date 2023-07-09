#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){

        if(argc!=3){
                //syslog(LOG_ERR,"");
                return 1;
        }

        const char *filename = argv[1];
        const char *text = argv[2];

        int file;

        file = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0755);

        if (write(file, text, strlen(text))==-1){
                close(file);
                return 1;
        }

        close(file);

        syslog(LOG_USER,"Writing %s to %s",text,filename);

        return 0;
}

