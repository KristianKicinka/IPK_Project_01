#ifndef SERVER_H
#define SERVER_H

#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>

#define PORT 8080
#define HOSTNAME_MAX_LEN 1024
#define PATH_MAX 256
#define CK_TIME 1
#define EXIT_CODE 1
#define SOCKET_BUFFER_SIZE 1024
#define GET_DOMAIN_NAME "GET /hostname"
#define GET_CPU_LOAD "GET /load"
#define GET_CPU_NAME "GET /cpu-name"
#define ERR_400 "400 Bad Request"
#define HTTP_HEAD "HTTP/1.1 200 OK\r\nContent-Type:text/plain;\r\n\r\n"      

typedef struct cpu_usage_t{
    long int user;
    long int nice;
    long int sys;
    long int idle;
    long int iowait;
    long int irq;
    long int softirq;
    long int sum_usage;
    char cpu_buffer[PATH_MAX];
    char cpu_tag[PATH_MAX];
}CpuUsage;

typedef struct server_params_t{
    int socket_desc, new_socket;
    struct sockaddr_in socket_address_in;
    int option_val;
    int socket_addrlen;
    char socket_buffer[SOCKET_BUFFER_SIZE];
}ServerParams;


void initialize_server_params(ServerParams *params);

void initialize_CpuUsage(CpuUsage *cpu_usg);

void create_server_socket(ServerParams *params);

void bind_server_ports(ServerParams *params);

void set_server_socket(ServerParams *params);

void set_passive_mode(ServerParams *params);

void load_data(CpuUsage *cpu_usg, FILE *cp_file);

int get_port_number(int argc, char const *argv[]);

void send_bad_request(char* http_head, int new_socket);

void get_cpu_usage(char* http_head, int new_socket);

void get_processor_name(char* http_head, int new_socket);

void get_hostname(char* http_head, int new_socket);

#endif