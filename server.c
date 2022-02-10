#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>

#define PORT 8080

void get_cpu_usage(){
    long CPU_time = clock();
    long count_of_cores = sysconf(_SC_NPROCESSORS_ONLN);
    //long wall_time = clock_gettime();
    double cpu_usage = CPU_time / count_of_cores;
    printf("CPU USAGE : %f \n",cpu_usage);
}

int main(int argc, char const *argv[]){
    /* code */
    printf("sevas maroš\n");

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
