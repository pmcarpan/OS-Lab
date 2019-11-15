#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "response-codes.h"

#define MAX_CLIENTS 30
#define PORT 8888
#define MAX_SERVER_QUEUE_SIZE 10

#define BUFFER_SIZE 1024

const int INT_BOOLEAN_TRUE = 1; 

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

size_t get_len(int x) {
    if (x == 0) return (size_t) 1;
    size_t len = 0;
    while (x) { x /= 10; len++; }
    return len;
}

// get positive integer in [0, max)
int get_random_integer(int max) {
    return (int) ((((double) rand()) / RAND_MAX) * max);
}

char * get_dt_string() {
    time_t t; time(&t);
    char *time_str = asctime(localtime(&t));
    time_str[strlen(time_str) - 1] = '\0';
    return strdup(time_str);
}

char * get_weather_info() {
    char buffer[BUFFER_SIZE + 1];
    double temp = 20 + get_random_integer(1000) / 100.0,
           humidity = 20 + get_random_integer(8000) / 100.0,
           pres = 900 + get_random_integer(20000) / 100.0,
           rainfall = 0 + get_random_integer(1000) / 100.0,
           rainfall_chance = get_random_integer(10000) / 100.0;

    int num_write = snprintf(buffer, BUFFER_SIZE, "T%.2lfH%.2lfP%.2lfR%.2lfRC%.2lf", 
                             temp, humidity, pres, rainfall, rainfall_chance);
    if (num_write == -1) die("snprintf()");
    
    return strdup(buffer);
}

void proc(void) {
    int master_socket_fd;
    int client_socket_fd[MAX_CLIENTS];

    for (int i = 0; i < MAX_CLIENTS; i++) 
        client_socket_fd[i] = -1;

	if ((master_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
		die("socket()");
	}

	if (setsockopt(master_socket_fd, SOL_SOCKET, SO_REUSEADDR, 
        &INT_BOOLEAN_TRUE, sizeof(INT_BOOLEAN_TRUE)) == -1) { 
		die("setsockopt()"); 
	} 

    struct sockaddr_in address;
    address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 

    socklen_t address_len = sizeof(address);

	if (bind(master_socket_fd, (struct sockaddr *) &address, address_len) == -1) { 
		die("bind()");
	} 

    // set up the master socket as a passive port for listening
	if (listen(master_socket_fd, MAX_SERVER_QUEUE_SIZE) == -1) { 
		die("listen()");
	} 

	printf("Server established; listening on port %d\n", PORT);

    fd_set read_fd_set;
    char buffer[BUFFER_SIZE + 1];
    while (true) {
        FD_ZERO(&read_fd_set);
        FD_SET(master_socket_fd, &read_fd_set);

        int max_fd = master_socket_fd;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_socket_fd[i] != -1) {
                FD_SET(client_socket_fd[i], &read_fd_set);
            }
            if (client_socket_fd[i] > max_fd) {
                max_fd = client_socket_fd[i];
            }
        }

        int num_activity = select(max_fd + 1, &read_fd_set, NULL, NULL, NULL);

        char *dt_string = get_dt_string();
        printf("[%s] Server activity -- %d active port(s)\n", dt_string, num_activity);
        free(dt_string);

        if (num_activity == -1)     die("select()");        
        else if (num_activity == 0) continue;      

        // activity on master socket indicates new connection
        if (FD_ISSET(master_socket_fd, &read_fd_set)) {
            int new_socket_fd;
            if ((new_socket_fd = accept(master_socket_fd, (struct sockaddr *) &address, &address_len)) == -1) { 
				die("accept");
			} 
			
            dt_string = get_dt_string();
			printf("[%s] New client, FD: %d, ADDR: %s:%d\n", dt_string, new_socket_fd, inet_ntoa(address.sin_addr), ntohs(address.sin_port)); 
            free(dt_string);		

			// send new connection info 
            // char *ack_message = "ACK";
			// if( send(new_socket_fd, ack_message, strlen(ack_message), 0) != strlen(ack_message) ) { 
			// 	die("send()"); 
			// } 
				
			for (int i = 0; i < MAX_CLIENTS; i++) { 
				if (client_socket_fd[i] == -1) { 
					client_socket_fd[i] = new_socket_fd;	
					break; 
				} 
			} 

            num_activity--;
        }

        if (num_activity == 0) continue;

        // here, either a client has sent a message or it has disconnected
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int socket_fd = client_socket_fd[i];
            if (socket_fd == -1)                    continue;
            if (!FD_ISSET(socket_fd, &read_fd_set)) continue;
            
            // now attempt a read
            int num_read = read(socket_fd, buffer, BUFFER_SIZE);
            if (num_read == -1) {
                die("read()");
            }
            else if (num_read == 0) {
                getpeername(socket_fd, (struct sockaddr *) &address, &address_len);
                dt_string = get_dt_string(); 
				printf("[%s] Client disconnected, FD: %d, ADDR: %s:%d\n", dt_string, socket_fd, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                free(dt_string);
                if (close(socket_fd) == -1) die("close()");
                client_socket_fd[i] = -1;   
                continue;
            }

            // printf("DBG read [%s]\n", buffer);

            int response_code; char *msg = NULL;
            if (strncmp(buffer, "weather", num_read) == 0) {
                response_code = RESPONSE_CODE_SUCCESS;
                msg = get_weather_info();
            }
            else {
                response_code = RESPONSE_CODE_NOT_FOUND;
                msg = "NOT_FOUND";
            }

            dt_string = get_dt_string();
            int num_write = snprintf(buffer, BUFFER_SIZE, "%d\n%s\n%s", response_code, dt_string, msg);
            if (num_write < 0) die("snprintf()");

            // printf("$DBG %d // %lu\n", num_write, 2U + strlen(msg) + get_len(response_code));
            if ((size_t) num_write != strlen(msg) + strlen(dt_string) + 2U + get_len(response_code)) die("snprintf()");
            free(dt_string);
            if (response_code == RESPONSE_CODE_SUCCESS) free(msg);

            if (write(socket_fd, buffer, num_write) == -1) die("write()");
        }
    }
}

int main(void) {
    proc();

    return EXIT_SUCCESS;
}

