#include "authentication.h"

void generateSALT(char *salt){
    char hexadecimals[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    char c;

    for(int i = 0; i < HASH_LEN; i++){
        c = hexadecimals[rand() % 16];
        salt[i] = c;
    }
}

void generateHASH(char * salt, char * password,char * result){
    char concatenation[strlen(salt)+strlen(password)+1];

    sprintf(concatenation,"%s%s\0", password, salt);

    char* hash_command = malloc(MAX_BUFFER);
    sprintf(hash_command,"echo %s | sha256sum",concatenation);

    FILE* f = popen(hash_command,"r");
    
    if(fread(result,1,HASH_LEN,f) <0) {
        perror(result);
        exit(1);
    }

    pclose(f);
    free(hash_command);
    
    char result_tmp[HASH_LEN+1];
    strcpy(result_tmp,strtok(result," "));
    snprintf(result,HASH_LEN+1,"%s\0",result_tmp); // HERE
    
}

bool checkPassword(bank_account_t *bank_account, char * password){
    char tmp[HASH_LEN+1] = "";
    generateHASH(bank_account->salt, password, tmp);
    return (strcmp(tmp,bank_account->hash) == 0);
}
