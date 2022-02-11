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

#define PORT 8080
#define HOSTNAME_MAX_LEN 1024
#define PATH_MAX 256 


void get_processor_name();
double get_cpu_wall_time();
void get_cpu_usage();
void get_hostname();
void get_processor_name();



int main(int argc, char const *argv[]){

    get_cpu_usage();
    get_hostname();
    get_processor_name();

    /*char *welcome_message = "Welcome, from server";

    int socket_desc, new_socket;
    struct sockaddr_in socket_address_in;
    int option_val = 1;

    socket_desc = socket(AF_LOCAL,SOCK_STREAM,0);
    if(socket_desc == 0){
        fprintf(stderr,"Internal failature");
        exit(1);
    }

    if (setsocketopt(socket_desc,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option_val, sizeof(option_val))){
        fprintf(stderr,"Internal failature");
        exit(1);
    }

    socket_address_in.sin_family = AF_INET;
    socket_address_in.sin_addr.s_addr = INADDR_ANY;
    socket_address_in.sin_port = htons( PORT );

    if(bind(socket_desc,))

    */
    

    return 0;
}


double get_cpu_wall_time(){

    struct timeval time;
    if (gettimeofday(&time,NULL)){
        fprintf(stderr,"Get cpu walltime error!\n");
        return 0;
    }

    return (double) time.tv_sec + (double) time.tv_usec * 0.000001;
}

void get_cpu_usage(){

    double CPU_time = (double) clock() / CLOCKS_PER_SEC;
    double count_of_cores = sysconf(_SC_NPROCESSORS_ONLN);
    double wall_time = get_cpu_wall_time();

    printf("CPU TIME : %f \n",CPU_time);
    printf("Cores : %f \n",count_of_cores);
    printf("Wall time : %f \n",wall_time);
    double cpu_usage = CPU_time / count_of_cores / wall_time;
    printf("CPU USAGE : %f \n",cpu_usage);
}

void get_hostname(){

    char hostname[HOSTNAME_MAX_LEN];
    gethostname(hostname,HOSTNAME_MAX_LEN);
    printf("Hostname is : %s \n",hostname);

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
