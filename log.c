#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "log.h"

FILE *open_log(const char *path){
    FILE *server_log; //https://stackoverflow.com/questions/23856306/how-to-create-log-file-in-c

    server_log = fopen(path, "a");
    if ( server_log == NULL )
    {
        printf("Error while opening logfile. \n");
        exit(EXIT_FAILURE);
    }
    return server_log;
}

void get_time(FILE *server_log){
	time_t t;
	struct tm *tm;
	t = time(NULL);
    tm = localtime(&t);

	fprintf(server_log, "[%04d-%02d-%02d %02d:%02d:%02d]: ",
	    tm->tm_year+1900,
	    tm->tm_mon+1,
	    tm->tm_mday,
	    tm->tm_hour,
	    tm->tm_min,
	    tm->tm_sec
    );
}

void Log(FILE *server_log, char *msg){
 	get_time(server_log);
 	fprintf(server_log, "%s", msg);

    printf("%s", msg);
}

void close_log(FILE *fp){
    fclose(fp);
    if (fclose(fp) != 0) {
        perror("ERROR: Not able to close server log.");
    } 
}