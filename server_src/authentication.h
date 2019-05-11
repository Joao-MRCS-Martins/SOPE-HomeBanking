#include "../auxiliary_code/sope.h"
#include <string.h>
#include <stdlib.h>

char* generateSALT();
char* generateHASH(char * salt, char * password);
bool checkPassword(bank_account_t *bank_account, char * password);
