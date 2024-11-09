#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

int parseline(char *buf, char *argv[], int argc);
int builtin_command(char *argv[]);
void execve_command(char **argv);

#endif