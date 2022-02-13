/**
 * 
 * Project : IPK Projekt 1 (vytvoření serveru v jazyce C/C++).
 * 
 * @file server.c 
 * @author Kristián Kičinka (xkicin02)
 * @brief  Server v jazyku C (implementácia)
 * 
 */

#include "server.h"


int main(int argc, char const *argv[]){
    ServerParams sv_params;
    initialize_server_params(&sv_params);

    int port_number = get_port_number(argc,argv);

    printf("Server is running on http://localhost:%d/\n", port_number);

    create_server_socket(&sv_params);
    set_server_socket(&sv_params);
    bind_server_ports(&sv_params);
    set_passive_mode(&sv_params);

    while(true){

        if ((sv_params.new_socket = accept(sv_params.socket_desc, (struct sockaddr *)&sv_params.socket_address_in, (socklen_t*)&sv_params.socket_addrlen)) < 0){
            fprintf(stderr, "Accept failed!\n");
            exit(EXIT_CODE);
        }

        read( sv_params.new_socket , sv_params.socket_buffer, SOCKET_BUFFER_SIZE);
        char *parsed_buffer = strtok(sv_params.socket_buffer, "\n");
        char *http_head = malloc(strlen(HTTP_HEAD) + HOSTNAME_MAX_LEN);
        strcpy(http_head, HTTP_HEAD);

        if(strstr(parsed_buffer, GET_DOMAIN_NAME)){
            get_hostname(http_head, sv_params.new_socket);
        }else if(strstr(parsed_buffer, GET_CPU_NAME)){
            get_processor_name(http_head, sv_params.new_socket);
        }else if(strstr(parsed_buffer, GET_CPU_LOAD)){
            get_cpu_usage(http_head, sv_params.new_socket);
        }else{
            send_bad_request(http_head, sv_params.new_socket);
        }
        close(sv_params.new_socket);
    }
    return 0;
}

int get_port_number(int argc, char const *argv[]){
    int port_number;

    if (argc > 1){
        char *ptr;
        port_number = strtol(argv[1], &ptr, 10);

        if(*ptr != '\0'){
            fprintf(stderr,"Corrupted port number!\n");
            exit(EXIT_CODE);
        }
        return port_number;
    }else{
        fprintf(stderr,"No port selected!\n");
        exit(EXIT_CODE);
    }
}

void initialize_server_params(ServerParams *params){
    params->option_val = 1;
    params->socket_addrlen = sizeof(params->socket_address_in);
}

void create_server_socket(ServerParams *params){
    params->socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(params->socket_desc == 0){
        fprintf(stderr,"Socket inicialization failure!\n");
        exit(EXIT_CODE);
    }
}

void bind_server_ports(ServerParams *params){
    if (bind(params->socket_desc, (struct sockaddr *)&params->socket_address_in, sizeof(params->socket_address_in)) < 0){
        fprintf(stderr,"Bind failed!\n");
        exit(EXIT_CODE);
    }
}

void set_server_socket(ServerParams *params){
    if (setsockopt(params->socket_desc,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &params->option_val, sizeof(params->option_val))){
        fprintf(stderr,"Internal failure!\n");
        exit(EXIT_CODE);
    }

    params->socket_address_in.sin_family = AF_INET;
    params->socket_address_in.sin_addr.s_addr =  htonl(INADDR_ANY);
    params->socket_address_in.sin_port = htons( PORT );
}

void set_passive_mode(ServerParams *params){
    if (listen(params->socket_desc, 3) < 0){
        fprintf(stderr, "Listening failed!\n");
        exit(EXIT_CODE);
    }
}

void send_bad_request(char* http_head, int new_socket){
    strcat(http_head, ERR_400);
    send(new_socket , http_head , strlen(http_head) , 0 );
}

void initialize_CpuUsage(CpuUsage *cpu_usg){
    cpu_usg->user = cpu_usg->nice = cpu_usg->sys = cpu_usg->idle = cpu_usg->iowait = cpu_usg->irq = 0;
    cpu_usg->softirq = cpu_usg->sum_usage = 0;
    memset(cpu_usg->cpu_buffer, 0, sizeof(cpu_usg->cpu_buffer));
    cpu_usg->cpu_tag[0] = '\0'; 
}

void load_data(CpuUsage *cpu_usg, FILE *cp_file){
    fgets(cpu_usg->cpu_buffer, sizeof(cpu_usg->cpu_buffer), cp_file);
    sscanf(cpu_usg->cpu_buffer,"%s%ld%ld%ld%ld%ld%ld%ld", cpu_usg->cpu_tag,&cpu_usg->user,&cpu_usg->nice,&cpu_usg->sys,&cpu_usg->idle,&cpu_usg->iowait,&cpu_usg->irq,&cpu_usg->softirq);
    cpu_usg->sum_usage = cpu_usg->user+cpu_usg->nice+cpu_usg->sys+cpu_usg->idle+cpu_usg->iowait+cpu_usg->irq+cpu_usg->softirq;
}

void get_cpu_usage(char* http_head, int new_socket){
    CpuUsage cpu_prev;
    CpuUsage cpu_next;
    double cpu_usage;
    char percentage_tag = '%';
    char usage_str[HOSTNAME_MAX_LEN];

    initialize_CpuUsage(&cpu_prev);
    initialize_CpuUsage(&cpu_next);

    FILE *cpu_file;

    while(true){

        cpu_file = fopen("/proc/stat", "r");
        if(cpu_file == NULL){
            fprintf(stderr, "CPU file not found!\n");
            return;
        }

        load_data(&cpu_prev, cpu_file);

        rewind(cpu_file);
        sleep(CK_TIME);

        load_data(&cpu_next, cpu_file);

        cpu_usage = (double)(cpu_next.sum_usage - cpu_prev.sum_usage - (cpu_next.idle - cpu_prev.idle)) / (cpu_next.sum_usage - cpu_prev.sum_usage) * 100;

        fclose(cpu_file);  

        snprintf(usage_str, HOSTNAME_MAX_LEN, "%.2f", cpu_usage);
        strncat(http_head, usage_str, strlen(usage_str));
        strncat(http_head, &percentage_tag, sizeof(char));
        send(new_socket, http_head, strlen(http_head), 0);

        break;
    }

}

void get_hostname(char* http_head, int new_socket){
    char hostname[HOSTNAME_MAX_LEN];

    gethostname(hostname, HOSTNAME_MAX_LEN);
    strncat(http_head, hostname, strlen(hostname));
    send(new_socket, http_head, strlen(http_head), 0);
}

void get_processor_name(char* http_head, int new_socket){
    FILE *cpu_name_fp;
    char cpu_name[PATH_MAX];

    cpu_name_fp = popen("lscpu | grep 'Model name:' | sed -r 's/Model name:\\s{1,}//g'", "r");

    if (cpu_name_fp == NULL){
        fprintf(stderr, "CPU file not found!\n");
        return;
    }

    fgets(cpu_name, PATH_MAX, cpu_name_fp);

    strncat(http_head, cpu_name, strlen(cpu_name));
    send(new_socket, http_head, strlen(http_head), 0);

    pclose(cpu_name_fp);
}
