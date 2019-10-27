#include <sys/types.h>

struct process{
    pid_t pid;
    pid_t pgid;
    int background;
};

typedef struct process process;

struct pControl
{
    process **process;
    int length;
    int max;
};

typedef struct pControl pControl;

pControl VetProcessinit();
pControl VetProcessinsertItem(pControl, process*);
pControl VetProcessremoveItem(pControl, process*);
process* VetProcessSearch(pControl, pid_t);
void VetProcessfree(pControl);
process* VetProcessNext(pControl,process*);