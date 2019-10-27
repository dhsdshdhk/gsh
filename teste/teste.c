#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>

int main(){
    while(1){
        printf("1\n");
        sleep(10);
    }
}