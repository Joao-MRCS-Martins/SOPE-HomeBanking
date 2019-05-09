#include "../auxiliary_code/sope.h"
#include <math.h>
#include <string.h>

void open_server(char * password, int bank_id);
void close_server(int bank_id);
char* generateSALT();
char* generateHASH(char * salt, char * password);
bool checkPassword(char * password);
