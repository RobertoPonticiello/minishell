#ifndef MINISHELL_H
#define MINISHELL_H

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

/* --- Struct globale unico per stato shell --- */
typedef struct s_shell_state
{
    volatile sig_atomic_t signal;
    int last_status;
}   t_shell_state;

typedef struct s_command {
    char   *path;       // percorso dell'eseguibile (già risolto)
    char  **argv;       // array di argomenti, terminato da NULL
    int     in_fd;      // file descriptor da usare come STDIN (se non c'è, è -1)
    int     out_fd;     // file descriptor da usare come STDOUT (se non c'è, è -1)
    int     is_builtin; // 1 se è built-in, 0 altrimenti
    struct s_command *next; // puntatore al comando successivo (per pipeline)
} t_command;

typedef enum e_token_type {
    TOKEN_WORD,      // parola normale (comando o argomento)
    TOKEN_PIPE,      // |
    TOKEN_REDIR_IN,  // <
    TOKEN_REDIR_OUT, // >
    TOKEN_HEREDOC,   // <<
    TOKEN_APPEND,    // >>
    TOKEN_EOF        // fine input/token list
}   t_token_type;

typedef struct s_token {
    char           *value;      // testo del token
    t_token_type    type;       // tipo di token
    struct s_token *next;       // prossimo token nella lista
}   t_token;

static t_shell_state g_state = {0, 0};

/* --- Funzioni user (Parte esecuzione & segnali) --- */
void setup_signals(void);
void execute_pipeline(t_command *commands);
void cleanup_after_execution(t_command *commands);

/* --- Funzioni pipeline initialization --- */
int init_pipeline(t_command *commands, int **pipes, pid_t **pids);

/* --- Funzioni pipeline utilities --- */
void close_pipe_ends(int **pipes, int num_cmds, int current_cmd);
void setup_child_fds(t_command *cmd, int **pipes, int num_cmds, int i);
void execute_child(t_command *cmd);
void wait_for_children(pid_t *pids, int num_cmds);
void cleanup_resources(int **pipes, pid_t *pids, int num_cmds);
void close_last_pipe(int **pipes, int num_cmds);

/* --- Funzioni Persona 1 (Parte parsing) --- */
extern t_command *parse_input(const char *line);
extern void free_commands(t_command *commands);

#endif