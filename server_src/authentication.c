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
    char * result = malloc(HASH_LEN);

    strcpy(concatenation, password);
    strcat(concatenation, salt);

    char* hash_command = malloc(MAX_BUFFER);
    strcpy(hash_command,"echo ");
    strcat(hash_command,concatenation);
    strcat(hash_command," | sha256sum");
    FILE* f = popen(hash_command,"r");
    
    if(fread(result,1,HASH_LEN,f) <0) {
        perror(result);
        exit(1);
    }

    pclose(f);
    free(hash_command);
    free(concatenation);
    
    strcpy(result,strtok(result," "));
    
    return result;
}

bool checkPassword(bank_account_t *bank_account, char * password){
    char * result = generateHASH(bank_account->salt, password);
    
    return (strcmp(result,bank_account->hash) == 0);
}
