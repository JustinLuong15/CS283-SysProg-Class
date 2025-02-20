#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"
#include <errno.h>

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
static int last_rc = 0;

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer) {
        return ERR_MEMORY;
    }
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    cmd_buff->argc = 0;
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    memset(cmd_buff->_cmd_buffer, 0, SH_CMD_MAX);
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
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

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (cmd->argc == 0) {
        return BI_NOT_BI;
    }

    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc == 1) {
            last_rc = 0;
            return BI_CMD_CD;
        } else if (cmd->argc == 2) {
            if (chdir(cmd->argv[1]) != 0) {
                last_rc = errno;
                perror("cd");
            } else {
                last_rc = 0;
            }
            return BI_CMD_CD;
        } else {
            last_rc = E2BIG;
            fprintf(stderr, "cd: too many arguments\n");
            return BI_CMD_CD;
        }
    } else if (strcmp(cmd->argv[0], "exit") == 0) {
        return BI_CMD_EXIT;
    } else if (strcmp(cmd->argv[0], "rc") == 0) {
        printf("%d\n", last_rc);
        return BI_CMD_RC;
    }

    return BI_NOT_BI;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    clear_cmd_buff(cmd_buff);

    char *input = cmd_buff->_cmd_buffer;
    strncpy(input, cmd_line, SH_CMD_MAX - 1);
    input[SH_CMD_MAX - 1] = '\0';

    int argc = 0;
    char *current = input;
    char *start = input;
    int in_quotes = 0;
    char quote_char = 0;

    while (*current) {
        if (in_quotes) {
            if (*current == quote_char) {
                in_quotes = 0;
                *current = '\0';
                cmd_buff->argv[argc++] = start;
                start = current + 1;
            }
            current++;
        } else {
            if (*current == '"' || *current == '\'') {
                in_quotes = 1;
                quote_char = *current;
                *current = '\0';
                if (current > start) {
                    cmd_buff->argv[argc++] = start;
                }
                start = current + 1;
                current++;
            } else if (isspace(*current)) {
                *current = '\0';
                if (current > start) {
                    cmd_buff->argv[argc++] = start;
                }
                start = current + 1;
                current++;
            } else {
                current++;
            }
        }
    }

    if (current > start) {
        cmd_buff->argv[argc++] = start;
    }

    cmd_buff->argc = argc;
    return (argc == 0) ? WARN_NO_CMDS : OK;
}

int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return ERR_EXEC_CMD;
    } else if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
        switch (errno) {
            case ENOENT:
                fprintf(stderr, "Command not found in PATH\n");
                break;
            case EACCES:
                fprintf(stderr, "Permission denied\n");
                break;
            default:
                fprintf(stderr, CMD_ERR_EXECUTE ": %s\n", strerror(errno));
        }
        exit(errno);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            last_rc = WEXITSTATUS(status);
        } else {
            last_rc = -1;
        }
    }
    return OK;
}

int exec_local_cmd_loop() {
    char cmd_buff_line[SH_CMD_MAX];
    cmd_buff_t cmd;

    if (alloc_cmd_buff(&cmd) != OK) {
        fprintf(stderr, "Failed to allocate command buffer\n");
        return ERR_MEMORY;
    }

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff_line, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        cmd_buff_line[strcspn(cmd_buff_line, "\n")] = '\0';

        int rc = build_cmd_buff(cmd_buff_line, &cmd);
        switch (rc) {
            case WARN_NO_CMDS:
                printf(CMD_WARN_NO_CMD);
                continue;
            case ERR_TOO_MANY_COMMANDS:
                printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
                continue;
            case OK:
                break;
            default:
                continue;
        }

        Built_In_Cmds bic = match_command(cmd.argv[0]);
        if (bic != BI_NOT_BI) {
            Built_In_Cmds result = exec_built_in_cmd(&cmd);
            if (result == BI_CMD_EXIT) {
                free_cmd_buff(&cmd);
                return OK_EXIT;
            }
        } else {
            if (cmd.argc == 0) {
                continue;
            }
            exec_cmd(&cmd);
        }
    }

    free_cmd_buff(&cmd);
    return OK;
}