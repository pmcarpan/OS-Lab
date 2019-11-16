#include <stdbool.h>
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/select.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 

#include "text-styles.h"


#define S_NAME "sfifo"
#define R_NAME "rfifo"

#define MAX(a, b) ( ((a) >= (b)) ? (a) : (b) )

int main(void) { 
    printf(SCREEN_CLEARING_TEXT); fflush(stdout);

	int s_fd, r_fd; 

	mkfifo(S_NAME, 0666);
    mkfifo(R_NAME, 0666); 


    fd_set rd_set, wr_set;
	while (true) {
        s_fd = open(S_NAME, O_WRONLY);		
        r_fd = open(R_NAME, O_RDONLY); 

        FD_ZERO(&rd_set); FD_ZERO(&wr_set); 
        FD_SET(r_fd, &rd_set);
        FD_SET(STDIN_FILENO, &rd_set);

        int max_fd = MAX(r_fd, STDIN_FILENO), 
            num_activity = select(max_fd + 1, &rd_set, NULL, NULL, NULL);

        if (num_activity == 0) continue;

        if (FD_ISSET(r_fd, &rd_set)) {
            char buf[128];
            ssize_t num = read(r_fd, buf, 127);
            buf[num - 1] = '\0';
            printf("%s%s> [Sender]%s %s%s%s\n", ANSI_STYLE_BOLD, ANSI_COLOR_GREEN, ANSI_RESET, ANSI_STYLE_ITALIC, buf, ANSI_RESET);
        }

        if (FD_ISSET(STDIN_FILENO, &rd_set)) {
            char buf[128];
            ssize_t num = read(STDIN_FILENO, buf, 127);
            write(s_fd, buf, num);
        }

        close(s_fd);
        close(r_fd);
	} 
	
    return 0; 
} 

