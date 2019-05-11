#include "authentication.h"

char* generateSALT(){
    char hexadecimals[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    char * salt = malloc (SALT_LEN + 1);
    char c;

    for(int i = 0; i < HASH_LEN; i++){
        c = hexadecimals[rand() % 16];
        salt[i] = c;
    }

    return salt;
}

char* generateHASH(char * salt, char * password){
    char * concatenation = malloc(sizeof(salt) + sizeof(password));
    char * result = malloc(HASH_LEN + 1);

    strcpy(concatenation, password);
    strcat(concatenation, salt);

    printf("%s\n", concatenation);

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

bool checkPassword(bank_account_t *bank_account, char * password){
    char * result = generateHASH(bank_account->salt, password);

    for(int i = 0; i < HASH_LEN; i++){
        if(result[i] != bank_account->hash[i]){
            return false;
        }
    }

    return true;
}
