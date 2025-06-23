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

extern t_shell_state g_state;

/* --- Funzioni user (Parte esecuzione & segnali) --- */
void setup_signals(void);
int execute_pipeline(t_command *commands);
int count_commands(t_command *commands);
void cleanup_after_execution(t_command *commands);

/* --- Funzioni Persona 1 (Parte parsing) --- */
extern t_command *parse_input(const char *line);
extern void free_commands(t_command *commands);

/* --- Funzioni di tokenizzazione --- */
t_token *tokenize(const char *input);
void free_tokens(t_token *tokens);

/* --- Funzioni di gestione token --- */
t_token *create_token(const char *value, t_token_type type);
t_token *handle_redirection_operator(const char *input, int *i);
int is_operator(const char *input, int i);
char *extract_quoted_word(const char *input, int *i, char quote);
char *extract_word(const char *input, int *i);

/* --- Funzioni di gestione quote --- */
void handle_quotes(t_token *token);
int process_escape(const char *str, int *i, char *result, int *j);

/* --- Funzioni di gestione variabili --- */
int copy_env_value(const char *src, int *si, char *dst, int *di);
void expand_variables(t_token **tokens);
char *expand_string(const char *str);

/* --- Funzioni di controllo sintassi --- */
int check_syntax_errors(t_token *tokens);

/* --- Funzioni di gestione redirezioni --- */
void handle_redirection(t_command *cmd, t_token *curr);

/* --- Funzioni di costruzione comandi --- */
t_command *build_commands(t_token *tokens);

/* --- Funzioni di risoluzione percorsi --- */
char *find_executable(const char *cmd);

/* --- Funzioni built-in --- */
int ft_echo(char **args);
int ft_cd(char **args);
int ft_pwd(char **args);
int ft_export(char **args);
int ft_unset(char **args);
int ft_env(char **args);
int ft_exit(char **args);

/* --- Funzioni di pipeline --- */
int init_pipeline(t_command *commands, int ***pipes, pid_t **pids);
void cleanup_resources(int **pipes, pid_t *pids, int num_cmds);
void setup_child_pipes(t_command *cmd, int **pipes, int num_cmds, int i);
void execute_child(t_command *cmd);
void wait_for_children(pid_t *pids, int num_cmds);
int **create_pipes(int num_cmds);

void execute_builtin(t_command *cmd);

#endif