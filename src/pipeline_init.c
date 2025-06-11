#include "minishell.h"

static int count_commands(t_command *commands)
{
    int count = 0;
    t_command *cmd = commands;
    
    while (cmd) {
        count++;
        cmd = cmd->next;
    }
    return count;
}

static int **create_pipes(int num_cmds)
{
    int **pipes;
    int i;

    if (num_cmds <= 1)
        return NULL;
    
    pipes = malloc(sizeof(int *) * (num_cmds - 1));
    if (!pipes)
        return NULL;
    
    for (i = 0; i < num_cmds - 1; ++i) {
        pipes[i] = malloc(sizeof(int) * 2);
        if (!pipes[i] || pipe(pipes[i]) < 0) {
            perror("pipe");
            while (--i >= 0)
                free(pipes[i]);
            free(pipes);
            return NULL;
        }
    }
    return pipes;
}

int init_pipeline(t_command *commands, int **pipes, pid_t **pids)
{
    int num_cmds = count_commands(commands);
    if (num_cmds == 0)
        return 0;

    *pipes = create_pipes(num_cmds);
    if (num_cmds > 1 && !*pipes)
        return 0;

    *pids = malloc(sizeof(pid_t) * num_cmds);
    if (!*pids) {
        cleanup_resources(*pipes, NULL, num_cmds);
        return 0;
    }
    return num_cmds;
} 