#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "response-codes.h"

#define PORT 8888
#define BUFFER_SIZE 1024

#define SLEEP_INTERVAL 3

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void disp_decoded_weather_info(char *w_str) {
    double temp,
           humidity,
           pres,
           rainfall,
           rainfall_chance;

    int num_read = sscanf(w_str, "T%lfH%lfP%lfR%lfRC%lf", 
                          &temp, &humidity, &pres, &rainfall, &rainfall_chance);
    // printf("DBG %d\n", num_read);
    if (num_read == EOF || num_read != 5) die("sscanf()");

    printf("%-20s%10.2f\n%-20s%10.2f\n%-20s%10.2f\n%-20s%10.2f\n%-20s%10.2f\n", 
           "Temperature (C)", temp, "Humidity (%)", humidity, "Pressure (mbar)", pres, 
           "Rainfall (mm)", rainfall, "Rainfall Chance (%)", rainfall_chance);
}

void parse_info(char *msg) {
    char *w = strrchr(msg, '\n');
    w++;
    // printf("DBG %s\n", w);
    printf("---------------------------------\n");
    disp_decoded_weather_info(w);
    printf("---------------------------------\n");
}

void proc(void) {
    struct sockaddr_in server_addr;
    struct hostent *he;
    int socket_fd;
    char buffer[BUFFER_SIZE + 1];

    if ((he = gethostbyname("localhost")) == NULL) {
        die("gethostbyname()");
    }

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        die("socket()");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr = *((struct in_addr *) he->h_addr_list[0]);

    if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        die("connect()");
    }

    while (1) {
        int num_sent = write(socket_fd, "weather", strlen("weather"));
        if (num_sent == -1) die("write()");
        else if (num_sent != strlen("weather")) die("write()");

        int num_recd = read(socket_fd, buffer, BUFFER_SIZE);
        if (num_recd == -1) die("read()");

        buffer[num_recd] = '\0';
        printf("\n\nWeather info recd: [%s]\n", buffer);

        parse_info(buffer);

        printf("Sleep for %d sec\n", SLEEP_INTERVAL);
        sleep(SLEEP_INTERVAL);
    }
}

int main(void) {
    proc();

    return EXIT_SUCCESS;
}

