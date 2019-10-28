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
#include <string.h>
#include "vetProcess.h"

char *strip(char *command);
void loop();
char *read_line();
char **parse_line(char *line);
char **split_command(char *command);
int execute(char **commands);
void sigintHandle(int sig);
void sigchldHandle(int);
void sigtstphandle(int);
void cleanDie();

pControl processControl;

struct sigaction act;

int main(int argc, char **argv)
{
    processControl = VetProcessinit();

    if (signal(SIGINT, sigintHandle) == SIG_ERR)
    {
        printf("Erro ao instalar o sinal");
    }
    if (signal(SIGCHLD, sigchldHandle) == SIG_ERR)
    {
        printf("Erro ao instalar o sinal");
    }
    if (signal(SIGTSTP, sigtstphandle) == SIG_ERR)
    {
        printf("Erro ao instalar o sinal");
    }
    loop();
    return 0;
}

void loop()
{
    char *line;
    char **commands;
    int status = 1;

    do
    {
        fflush(stdout);
        printf("gsh> ");
        fflush(stdout);

        line = read_line();
        if (strlen(strip(line)) == 0)
        {
            continue;
        }
        
        commands = parse_line(line);
        status = execute(commands);

        free(line);
        free(commands);
    } while (status);
}
// Lê a entrada da linha de comando e retorna a entrada.
char *read_line()
{
    char *line = NULL;
    ssize_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    return line;
}

// Recebe a entrada da linha de comando e retorna um array de commandos, terminado com NULL.
char **parse_line(char *line)
{
    int i = 0;
    char **commands = malloc(6 * sizeof(char *));
    char *command;

    while ((command = strsep(&line, "#")) != NULL)
    {
        commands[i++] = strip(command);
        if (i == 5)
        {
            break;
        }
    }

    commands[i] = NULL;

    return commands;
}
// Recebe um comando e retorna um array de tokens pra usar no execvp.
char **split_command(char *command)
{
    int i = 0;
    char **args = malloc(4 * sizeof(char *));
    char *arg;

    while ((arg = strsep(&command, " ")) != NULL)
    {
        args[i++] = arg;
        if (i == 3)
        {
            break;
        }
    }
    args[i] = NULL;
    return args;
}

// Recebe array de comandos e retorna número de comandos.
int len_commands(char **commands)
{
    int c = 0;
    for (int i = 0; commands[i] != NULL; i++)
    {
        c++;
    }
    return c;
}

// Recebe array de comandos e os executa (ainda não pronto)
int execute(char **commands)
{
    srand(time(NULL));
    int c = len_commands(commands);
    pid_t pid[c], wpid, gpid = 0;
    int status[c];
    char **args;

    for (int i = 0; i < c; i++)
    {
        args = split_command(commands[i]);
        if (strcmp(args[0], "mywait") == 0)
        {
            printf("Essa bash não deixa zombies.\n");
            continue;
        }
        if (strcmp(args[0], "clean&die") == 0)
        {
            cleanDie();
        }
        pid[i] = fork();
        if (!pid[i])
        {
            if (rand() % 2)
            { // ghost
                fork();
                setpgid(getpid(), getppid());
            }
            signal(SIGINT, SIG_IGN);
            if (c > 1)
            {
                freopen("/dev/null", "r", stdin);
                freopen("/dev/null", "w", stdout);
            }
            execvp(args[0], args);
        }
        else
        {
            if (c > 1)
            {
                if (gpid == 0)
                {
                    gpid = pid[i];
                }
                setpgid(pid[i], gpid);
                process *p = malloc(sizeof(process));
                p->pid = pid[i];
                p->pgid = gpid;
                p->background = 1;
                processControl = VetProcessinsertItem(processControl, p);
            }
        }
    }
    
    if (c <= 1)
    { // quando apenas 1 comando, executar foreground
        process *p = malloc(sizeof(process));
        p->pid = pid[0];
        p->pgid = 0;
        p->background = 0;
        processControl = VetProcessinsertItem(processControl, p);
        wait(NULL);
    }

    return 1;
}

char *strip(char *command)
{
    char *str = malloc(100);
    strcpy(str, command);

    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0) // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

void cleanDie()
{
    process *p = NULL;
    while (p = VetProcessNext(processControl, p))
    {
        if(p->background == 1){
            kill(-(p->pid), SIGKILL);
        }
        kill(p->pid, SIGKILL);
    }
    VetProcessfree(processControl);
    exit(0);
}

void sigintHandle(int sig)
{
    char c;
    if (processControl.length > 0)
    {
        printf("Existem processos em execução, deseja finalizar a shell? (Y/n)");
        c = getchar();
        if (c == 'Y' || c == 'y')
        {
            process *p = NULL;
            while (p = VetProcessNext(processControl, p))
            {
                if(p->background){
                    kill(-(p->pid), SIGKILL);
                }
                kill(p->pid, SIGKILL);
            }
        }
        else
        {
            return;

        }
    }
    kill(-(getpid()), SIGKILL);
    VetProcessfree(processControl);
    exit(0);
}

void sigchldHandle(int sig)
{
    int status = 0;
    int pid = wait(&status);
    process *proc = VetProcessSearch(processControl, pid);
    if (proc == NULL)
    {
        return;
    }
    if (pid > 0)
    {
        if (WIFSIGNALED(status))
        {
            kill(-(proc->pgid), SIGKILL);
            kill(-(proc->pid), SIGKILL);
        }
    }
    processControl = VetProcessremoveItem(processControl, proc);
}

void sigtstphandle(int sig)
{
    process *p = NULL;

    while (NULL != (p = VetProcessNext(processControl, p)))
    {

        if(p->background){
            kill(-(p->pid), SIGTSTP);
        }
        kill(p->pid, SIGTSTP);

    }
    fflush(stdout);
}
