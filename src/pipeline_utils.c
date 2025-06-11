#include "minishell.h"

void close_pipe_ends(int **pipes, int num_cmds, int current_cmd)
{
    if (current_cmd > 0) {
        close(pipes[current_cmd - 1][0]);
        close(pipes[current_cmd - 1][1]);
    }
}

void setup_child_fds(t_command *cmd, int **pipes, int num_cmds, int i)
{
    if (i > 0)
        dup2(pipes[i - 1][0], STDIN_FILENO);
    if (i < num_cmds - 1)
        dup2(pipes[i][1], STDOUT_FILENO);
    
    if (cmd->in_fd >= 0)
        dup2(cmd->in_fd, STDIN_FILENO);
    if (cmd->out_fd >= 0)
        dup2(cmd->out_fd, STDOUT_FILENO);
    
    for (int j = 0; j < num_cmds - 1; ++j) {
        close(pipes[j][0]);
        close(pipes[j][1]);
    }
}

void execute_child(t_command *cmd)
{
    if (cmd->is_builtin) {
        execute_builtin(cmd);
        exit(g_state.last_status);
    }
    execve(cmd->path, cmd->argv, environ);
    perror("execve");
    exit(EXIT_FAILURE);
}

void wait_for_children(pid_t *pids, int num_cmds)
{
    for (int k = 0; k < num_cmds; ++k) {
        int status;
        waitpid(pids[k], &status, 0);
        if (WIFEXITED(status))
            g_state.last_status = WEXITSTATUS(status);
        else if (WIFSIGNALED(status))
            g_state.last_status = 128 + WTERMSIG(status);
    }
}

void cleanup_resources(int **pipes, pid_t *pids, int num_cmds)
{
    if (num_cmds > 1) {
        for (int j = 0; j < num_cmds - 1; ++j)
            free(pipes[j]);
        free(pipes);
    }
    free(pids);
} 