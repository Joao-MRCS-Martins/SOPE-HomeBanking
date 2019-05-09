#include "logging_server.h"

extern struct bank_account admin;

//apos parsing
void open_server(char * password, int bank_id){
    admin.account_id = ADMIN_ACCOUNT_ID;
    admin.balance = 0;
    strcpy(admin.salt, generateSALT());
    strcpy(admin.hash, generateHASH(admin.salt, password));

    int fd = fopen(SERVER_LOGFILE, "w");
    pthread_t tid = pthread_self();

    logBankOfficeOpen(fd, bank_id, tid);

    close(fd);
}

void close_server(int bank_id){
    int fd = fopen(SERVER_LOGFILE, "a");
    pthread_t tid = pthread_self();

    logBankOfficeClose(fd, bank_id, tid);

    close(fd);
}

char* generateSALT(){
    char * salt = malloc (SALT_LEN + 1);
    long int max = pow(64*8,2);

    sprintf(salt, "%X", rand() % max);

    return salt;
}

char* generateHASH(char * salt, char * password){
    char * concatenation = malloc(sizeof(salt) + sizeof(password));
    char * result = malloc(HASH_LEN + 1);

    strcat(concatenation, password);
    strcpy(concatenation, salt);

    int pipe_des[2];

    if(pipe(pipe_des) != 0){
        perror("Failed to open pipe descriptors\n");  
    }

    pid_t pid = fork();

    if(pid == 0){
        close(pipe_des[0]);

        dup2(pipe_des[1], STDOUT_FILENO);

        execlp("echo", "echo", concatenation, NULL);
    }
    else{
        close(pipe_des[1]);

        int pipe_des2[2];
        
        pid = fork();

        if(pipe(pipe_des2) != 0){
            perror("Failed to open secondary pipe descriptors\n");  
        }

        if(pid == 0){
            close(pipe_des2[0]);

            dup2(pipe_des2[1], STDOUT_FILENO);

            execlp("sha256sum", "sha256sum", NULL);
        }
        else{
            close(pipe_des2[1]);

            read(pipe_des2[0], result, HASH_LEN);

            close(pipe_des2[0]);
        }

        close(pipe_des[0]);
    }

    return result;
}

bool checkPassword(char * password){
    char * result = generateHASH(admin.salt, password);

    for(int i = 0; i < HASH_LEN; i++){
        if(result[i] != admin.hash[i]){
            return false;
        }
    }

    return true;
}
