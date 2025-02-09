#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    clist->num = 0;
    if (cmd_line == NULL || *cmd_line == '\0') {
        return WARN_NO_CMDS;
    }

    char *str_store;
    char *token = strtok_r(cmd_line, PIPE_STRING, &str_store);
    int cmd_count = 0;

    while (token != NULL && cmd_count < CMD_MAX) {
        while (*token && isspace((unsigned char)*token)) {
            token++;
        }
        size_t len = strlen(token);
        while (len > 0 && isspace((unsigned char)token[len - 1])) {
            token[len - 1] = '\0';
            len--;
        }
        if (*token == '\0') {
            token = strtok_r(NULL, PIPE_STRING, &str_store);
            continue;
        }

        char *space_store;
        char *exe = strtok_r(token, " ", &space_store);
        if (exe == NULL) {
            token = strtok_r(NULL, PIPE_STRING, &str_store);
            continue;
        }

        strncpy(clist->commands[cmd_count].exe, exe, EXE_MAX - 1);
        clist->commands[cmd_count].exe[EXE_MAX - 1] = '\0';

        char args_buf[ARG_MAX];
        args_buf[0] = '\0';
        int first_arg = 1;
        char *arg;
        while ((arg = strtok_r(NULL, " ", &space_store)) != NULL) {
            if (!first_arg) {
                strncat(args_buf, " ", ARG_MAX - strlen(args_buf) - 1);
            }
            strncat(args_buf, arg, ARG_MAX - strlen(args_buf) - 1);
            first_arg = 0;
        }
        strncpy(clist->commands[cmd_count].args, args_buf, ARG_MAX - 1);
        clist->commands[cmd_count].args[ARG_MAX - 1] = '\0';

        cmd_count++;
        token = strtok_r(NULL, PIPE_STRING, &str_store);
    }

    if (token != NULL) {
        return ERR_TOO_MANY_COMMANDS;
    }

    if (cmd_count == 0) {
        return WARN_NO_CMDS;
    }

    clist->num = cmd_count;
    return OK;
}