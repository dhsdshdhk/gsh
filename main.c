#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

void loop();
char *read_line();
char **parse_line(char *line);
char **split_command(char *command);
int execute(char **commands);

int main(int argc, char **argv){
  loop();
  return 0;
  }

void loop(){
  char *line;
  char **commands;
  int status = 1;

  do {
    printf("gsh> ");
    line = read_line();
    commands = parse_line(line);
    status = execute(commands);
  } while (status);
}
// Lê a entrada da linha de comando e retorna a entrada.
char *read_line(){
  char *line = NULL;
  ssize_t bufsize = 0;
  getline(&line, &bufsize, stdin);
  return line;
  }

// Recebe a entrada da linha de comando e retorna um array de commandos, terminado com NULL.
char **parse_line(char *line){
    int i = 0;
    char **commands = malloc(5 * sizeof(char*));
    char *command;
    while ((command = strsep(&line, "#")) != NULL)
        commands[i++] = command;
    commands[i] = NULL;

    return commands;
}
// Recebe um comando e retorna um array de tokens pra usar no execvp.
char **split_command(char *command){
    int i = 0;
    char **args = malloc(10 * sizeof(char*));
    char *arg;
    while ((arg = strsep(&command, " ")) != NULL)
        args[i++] = arg;
    args[i] = NULL;
    return args;
}

// Recebe array de comandos e retorna número de comandos.
int len_commands(char **commands){
    int c = 0;
    for(int i = 0; commands[i] != NULL; i++)
        c++;
    return c;
}
// Recebe array de comandos e os executa (ainda não pronto)
int execute(char **commands){
    int c = len_commands(commands);
    pid_t pid[c], wpid;
    int status[c];
    char** args;

    for(int i = 0; i < c; i++){
        pid[i] = fork();
        if(!pid[i]){
            args = split_command(commands[i]);
            execvp(args[0], args);
            }
    }
    // espera os filhos morrerem.
    for(int i = 0; i < c; i++)
        waitpid(pid[i], &status[i], 0);

    return 1;
}