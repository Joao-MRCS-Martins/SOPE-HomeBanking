#include "../auxiliary_code/sope.h"
#include <string.h>
#include <stdlib.h>

char* generateSALT();
char* generateHASH(char * salt, char * password);
bool checkPassword(char* salt, char* hash, char * password);
