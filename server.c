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


void initialize_CpuUsage(CpuUsage *cpu_usg);
void load_data(CpuUsage *cpu_usg, FILE *cp_file);
void get_port_number(char *string, int argc, char const *argv[]);
void get_cpu_usage();
void get_processor_name();
void get_hostname(char* http_head, int new_socket);
void get_processor_name();

void get_port_number(char *string, int argc, char const *argv[]){

    if (argc>1){
        strcpy(string,argv[1]);
    }else{
        fprintf(stderr,"No port selected!\n");
        exit(1);
    }
}

int main(int argc, char const *argv[]){

    char port_number[PATH_MAX];
    get_port_number(port_number,argc,argv);
    printf("Port : %s\n",port_number);

    //get_processor_name();
    //get_hostname();
    //get_cpu_usage();


    //char *welcome_message = "Welcome, from server";

    int socket_desc, new_socket;
    struct sockaddr_in socket_address_in;
    int option_val = 1;
    int socket_addrlen = sizeof(socket_address_in);
    char socket_buffer[SOCKET_BUFFER_SIZE];

    socket_desc = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(socket_desc == 0){
        fprintf(stderr,"Internal failature");
        exit(EXIT_CODE);
    }

    if (setsockopt(socket_desc,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option_val, sizeof(option_val))){
        fprintf(stderr,"Internal failature");
        exit(EXIT_CODE);
    }

    socket_address_in.sin_family = AF_INET;
    socket_address_in.sin_addr.s_addr =  htonl(INADDR_ANY);
    socket_address_in.sin_port = htons( PORT );

    if (bind(socket_desc, (struct sockaddr *)&socket_address_in, sizeof(socket_address_in))<0){
        fprintf(stderr,"Bind failed!\n");
        exit(EXIT_CODE);
    }

    if (listen(socket_desc, 3) < 0){
        fprintf(stderr,"Listening failed!\n");
        exit(EXIT_CODE);
    }

    while(true){
        if ((new_socket = accept(socket_desc, (struct sockaddr *)&socket_address_in, (socklen_t*)&socket_addrlen))<0){
        fprintf(stderr,"Accept failed!\n");
        exit(EXIT_CODE);
        }

        read( new_socket , socket_buffer, SOCKET_BUFFER_SIZE);
        char *parsed_buffer = strtok(socket_buffer,"\n");
        // Parsed first row
        //char *http_head = "HTTP/1.1 200 OK\r\nContent-Type:text/plain;\r\n\r\n";
        char *http_head = malloc(HOSTNAME_MAX_LEN +256);

        strcpy(http_head,"HTTP/1.1 200 OK\r\nContent-Type:text/plain;\r\n\r\n");

        if(strstr(parsed_buffer,GET_DOMAIN_NAME)){
            get_hostname(http_head,new_socket);
        }else if(strstr(parsed_buffer,GET_CPU_NAME)){
            send(new_socket , ERR_400 , strlen(ERR_400) , 0 );
        }else if(strstr(parsed_buffer,GET_CPU_NAME)){
            send(new_socket , ERR_400 , strlen(ERR_400) , 0 );
        }else{
            send(new_socket , ERR_400 , strlen(ERR_400) , 0 );
        }

        close(new_socket);
        printf("%s\n",parsed_buffer);
        fflush(stdout);
    }

    

    return 0;
}


void initialize_CpuUsage(CpuUsage *cpu_usg){
    cpu_usg->user = cpu_usg->nice = cpu_usg->sys = cpu_usg->idle = cpu_usg->iowait = cpu_usg->irq = 0;
    cpu_usg->softirq = cpu_usg->sum_usage = 0;
    memset(cpu_usg->cpu_buffer,0,sizeof(cpu_usg->cpu_buffer));
    cpu_usg->cpu_tag[0] = '\0'; 
}

void load_data(CpuUsage *cpu_usg, FILE *cp_file){
    fgets(cpu_usg->cpu_buffer,sizeof(cpu_usg->cpu_buffer),cp_file);
    sscanf(cpu_usg->cpu_buffer,"%s%ld%ld%ld%ld%ld%ld%ld",cpu_usg->cpu_tag,&cpu_usg->user,&cpu_usg->nice,&cpu_usg->sys,&cpu_usg->idle,&cpu_usg->iowait,&cpu_usg->irq,&cpu_usg->softirq);
    cpu_usg->sum_usage = cpu_usg->user+cpu_usg->nice+cpu_usg->sys+cpu_usg->idle+cpu_usg->iowait+cpu_usg->irq+cpu_usg->softirq;
}

void get_cpu_usage(){

    CpuUsage cpu_prev;
    CpuUsage cpu_next;
    double cpu_usage;

    initialize_CpuUsage(&cpu_prev);
    initialize_CpuUsage(&cpu_next);

    FILE *cpu_file;

    while(true){

        cpu_file = fopen("/proc/stat", "r");
        if(cpu_file == NULL){
            fprintf(stderr,"CPU file not found!\n");
            return;
        }

        load_data(&cpu_prev,cpu_file);

        rewind(cpu_file);
        sleep(CK_TIME);

        load_data(&cpu_next,cpu_file);

        cpu_usage = (double)(cpu_next.sum_usage-cpu_prev.sum_usage - (cpu_next.idle-cpu_prev.idle)) / (cpu_next.sum_usage-cpu_prev.sum_usage) * 100;

        fclose(cpu_file);  
        printf("\rCPU usage is : %.2f%c",cpu_usage,'%');
        fflush(stdout);
    }

}

void get_hostname(char* http_head, int new_socket){
    char hostname[HOSTNAME_MAX_LEN];

    gethostname(hostname,HOSTNAME_MAX_LEN);
    strncat(http_head,hostname,strlen(hostname));
    printf("Hostname is : %s \n",hostname);
    printf("%s\n",http_head);
    send(new_socket , http_head , strlen(http_head) , 0 );
}

void get_processor_name(){

    FILE *cpu_name_fp;
    char cpu_name[PATH_MAX];

    cpu_name_fp = popen("lscpu | grep 'Model name:' | sed -r 's/Model name:\\s{1,}//g'", "r");

    if (cpu_name_fp == NULL){
        fprintf(stderr,"CPU file not found!\n");
        return;
    }

    fgets(cpu_name, PATH_MAX, cpu_name_fp);
    printf("%s", cpu_name);

    pclose(cpu_name_fp);
}
