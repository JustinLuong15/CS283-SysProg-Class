#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

#include "dshlib.h"

int last_rc = 0;
void build_argv_from_command(command_t *cmd, char *argv[], int *argc);

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    char cmd_line[SH_CMD_MAX];
    command_list_t cmd_list;

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_line, sizeof(cmd_line), stdin) == NULL) {
            printf("\n");
            break;
        }
        cmd_line[strcspn(cmd_line, "\n")] = '\0';

        if (strlen(cmd_line) == 0) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        if (strcmp(cmd_line, EXIT_CMD) == 0) {
            printf("exiting...\n");
            break;
        }

        int rc = build_cmd_list(cmd_line, &cmd_list);
        if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
            continue;
        } else if (rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;
        } else if (rc != OK) {
            continue;
        }

        if (cmd_list.num == 1) {
            Built_In_Cmds bic = match_command(cmd_list.commands[0].exe);
            if (bic != BI_NOT_BI) {
                if (bic == BI_CMD_EXIT) {
                    printf("exiting...\n");
                    break;
                }             else if (bic == BI_CMD_CD) {
                if (strlen(cmd_list.commands[0].args) > 0) {
                    char *args_copy = strdup(cmd_list.commands[0].args);
                    char *dir = strtok(args_copy, " ");
                    char *extra = strtok(NULL, " ");
                    if (extra != NULL) {
                        fprintf(stderr, "cd: too many arguments\n");
                        last_rc = E2BIG;
                    } else if (dir != NULL) {
                        if (chdir(dir) != 0) {
                            last_rc = errno;
                            perror("cd");
                        } else {
                            last_rc = 0;
                        }
                    }
                    free(args_copy);
                }
                continue;
            } else if (bic == BI_CMD_RC) {
                    printf("%d\n", last_rc);
                    continue;
                }
            }
        }

        execute_pipeline(&cmd_list);
    }

    return OK;
}

int execute_pipeline(command_list_t *clist) {
    int num_cmds = clist->num;
    pid_t pids[CMD_MAX];
    int prev_pipe_fd[2] = { -1, -1 };
    int pipe_fd[2];

    for (int i = 0; i < num_cmds; i++) {
        if (i < num_cmds - 1) {
            if (pipe(pipe_fd) < 0) {
                perror("pipe");
                return ERR_EXEC_CMD;
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return ERR_EXEC_CMD;
        } else if (pid == 0) {  
            if (i > 0) {
                if (dup2(prev_pipe_fd[0], STDIN_FILENO) < 0) {
                    perror("dup2");
                    exit(1);
                }
            }
            if (i < num_cmds - 1) {
                if (dup2(pipe_fd[1], STDOUT_FILENO) < 0) {
                    perror("dup2");
                    exit(1);
                }
            }
            if (i > 0) {
                close(prev_pipe_fd[0]);
                close(prev_pipe_fd[1]);
            }
            if (i < num_cmds - 1) {
                close(pipe_fd[0]);
                close(pipe_fd[1]);
            }

            char *argv[CMD_ARGV_MAX];
            int argc = 0;
            build_argv_from_command(&clist->commands[i], argv, &argc);

            execvp(argv[0], argv);
            if (errno == ENOENT) {
                fprintf(stderr, "Command not found in PATH\n");
            } else if (errno == EACCES) {
                fprintf(stderr, "Permission denied\n");
            } else {
                fprintf(stderr, "Execution error: %s\n", strerror(errno));
            }
            exit(errno);
        } else {  
            pids[i] = pid;
            if (i > 0) {
                close(prev_pipe_fd[0]);
                close(prev_pipe_fd[1]);
            }
            if (i < num_cmds - 1) {
                prev_pipe_fd[0] = pipe_fd[0];
                prev_pipe_fd[1] = pipe_fd[1];
                close(pipe_fd[1]);
            }
        }
    }

    int status;
    for (int i = 0; i < num_cmds; i++) {
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status)) {
            last_rc = WEXITSTATUS(status);
        } else {
            last_rc = -1;
        }
    }
    return OK;
}

void build_argv_from_command(command_t *cmd, char *argv[], int *argc) {
    *argc = 0;
    argv[(*argc)++] = cmd->exe;
    if (strlen(cmd->args) > 0) {
        size_t len = strlen(cmd->args);
        if ((cmd->args[0] == '"' && cmd->args[len - 1] == '"') ||
            (cmd->args[0] == '\'' && cmd->args[len - 1] == '\'')) {
            char *clean = malloc(len - 1);
            if (clean) {
                strncpy(clean, cmd->args + 1, len - 2);
                clean[len - 2] = '\0';
                argv[(*argc)++] = clean;
            }
        } else {
            char args_copy[ARG_MAX];
            strncpy(args_copy, cmd->args, ARG_MAX);
            args_copy[ARG_MAX - 1] = '\0';
            char *token = strtok(args_copy, " ");
            while (token != NULL && *argc < CMD_ARGV_MAX - 1) {
                argv[(*argc)++] = token;
                token = strtok(NULL, " ");
            }
        }
    }
    argv[*argc] = NULL;
}
int build_cmd_list(char *cmd_line, command_list_t *clist) {
    clist->num = 0;
    if (cmd_line == NULL || *cmd_line == '\0') {
        return WARN_NO_CMDS;
    }
    
    char *saveptr;
    int count = 0;
    char *cmd_token = strtok_r(cmd_line, PIPE_STRING, &saveptr);
    while (cmd_token != NULL && count < CMD_MAX) {
        while (*cmd_token && isspace((unsigned char)*cmd_token)) cmd_token++;
        char *end = cmd_token + strlen(cmd_token) - 1;
        while (end > cmd_token && isspace((unsigned char)*end)) {
            *end = '\0';
            end--;
        }
        if (*cmd_token == '\0') {
            cmd_token = strtok_r(NULL, PIPE_STRING, &saveptr);
            continue;
        }
        
        char exe[EXE_MAX] = {0};
        char args[ARG_MAX] = {0};
        int i = 0, j = 0;
        bool in_quotes = false;
        char quote_char = '\0';
        while (cmd_token[i] != '\0') {
            char c = cmd_token[i];
            if (!in_quotes && (c == '"' || c == '\'')) {
                in_quotes = true;
                quote_char = c;
                i++;
                continue;
            }
            if (in_quotes && c == quote_char) {
                in_quotes = false;
                i++;
                continue;
            }
            if (!in_quotes && isspace((unsigned char)c))
                break;
            if (j < EXE_MAX - 1)
                exe[j++] = c;
            i++;
        }
        exe[j] = '\0';
        
        while (cmd_token[i] != '\0' && isspace((unsigned char)cmd_token[i])) {
            i++;
        }
        strncpy(args, cmd_token + i, ARG_MAX - 1);
        args[ARG_MAX - 1] = '\0';
        
        strncpy(clist->commands[count].exe, exe, EXE_MAX - 1);
        clist->commands[count].exe[EXE_MAX - 1] = '\0';
        strncpy(clist->commands[count].args, args, ARG_MAX - 1);
        clist->commands[count].args[ARG_MAX - 1] = '\0';
        
        count++;
        cmd_token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }
    
    if (count == 0) {
        return WARN_NO_CMDS;
    }
    
    clist->num = count;
    return OK;
}


Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, EXIT_CMD) == 0) {
        return BI_CMD_EXIT;
    } else if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    } else if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    } else if (strcmp(input, "rc") == 0) {
        return BI_CMD_RC;
    } else {
        return BI_NOT_BI;
    }
}
