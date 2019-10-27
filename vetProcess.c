#include "vetProcess.h"
#include <stdlib.h>

pControl VetProcessinit(){
    pControl p;
    int i;
    p.process = malloc(10 * sizeof(process));
    p.length = 0;
    p.max = 10;
    for(i = 0; i < p.max; i++){
        p.process[i] = NULL;
    }
    return p;
}

pControl VetProcessinsertItem(pControl pc, process *p){
    int i;
    if(pc.length == pc.max){
        pc.process = realloc(pc.process, pc.max*2*sizeof(process));
        pc.max = pc.max*2;
    }
    for(i = 0; i < pc.max; i++){
        if(pc.process[i] == NULL){
            pc.process[i] = p;
            pc.length += 1;
            break;
        }
    }
    return pc;
}

pControl VetProcessremoveItem(pControl pc, process *p){
    int i = 0;
    for(i = 0;i < pc.max; i++){
        if(pc.process[i] == p){
            pc.process[i] = NULL;
            pc.length -= 1;
            free(pc.process[i]);
            break;
        }
    }
    return pc;
}

void VetProcessfree(pControl p){
    int i;
    for(i = 0; i < p.max; i++){
        free(p.process[i]);
    }
    free(p.process);
}

process* VetProcessSearch(pControl p, pid_t pid){
    int i = 0;
    for(i = 0; i < p.max; i++){
        if(p.process[i] == NULL){
            continue;
        }
        if(p.process[i]->pid == pid){
            return p.process[i];
        }
    }
    return NULL;
}

process* VetProcessNext(pControl pc, process *p){
    int i;
    for(i = 0; i < pc.max; i++){
        if(pc.process[i] != NULL && p == NULL){
            return pc.process[i];
        }
        if(pc.process[i] == p){
            p = NULL;
        }
    }
    return NULL;
}