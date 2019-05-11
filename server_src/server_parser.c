#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "server_parser.h"
#include "authentication.h"

int input_parser(char* args[],bank_account_t * admin, int* nthr) {
    *nthr = atoi(args[1]);
    char pass[MAX_PASSWORD_LEN+1];

    if(*nthr < 0 || *nthr > MAX_BANK_OFFICES) {
        printf("Number of e-counters must be in range: 0-%d\n",MAX_BANK_OFFICES);
        return RC_LOGIN_FAIL;
    }

    if(strlen(args[2]) < MIN_PASSWORD_LEN || strlen(args[2]) > MAX_PASSWORD_LEN) {
        printf("Password must be between 8 and 20 characters long.\n");
        return RC_LOGIN_FAIL;
    }

    sprintf(pass,strtok(args[2]," "));
    if(strtok(NULL," ") != NULL) {
        printf("Password can't contain spaces.\n");
        return RC_OP_NALLOW;
    }

    admin->account_id = ADMIN_ACCOUNT_ID;
    admin->balance = 0;
    strcpy(admin->salt, generateSALT());
    strcpy(admin->hash, generateHASH(admin->salt, pass));
    
    return RC_OK;
}