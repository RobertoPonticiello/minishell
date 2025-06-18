#include "minishell.h"  // contiene t_shell_state g_state e prototipi
#include "libft.h"     // per ft_putstr_fd
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

extern char **environ;  // Dichiarazione esterna di environ

/*
  Esegue una lista di comandi collegati da pipe e redirezioni.
  commands è la testa di una lista collegata (linked list) di t_command.
*/
int execute_pipeline(t_command *commands)
{
    int **pipes;
    pid_t *pids;
    int num_cmds = count_commands(commands);

    // Se è un singolo comando built-in, eseguilo direttamente
    if (num_cmds == 1 && commands->is_builtin) {
        execute_builtin(commands);
        return g_state.last_status;
    }

    // Altrimenti procedi con la pipeline normale
    num_cmds = init_pipeline(commands, &pipes, &pids);
    int i;
    t_command *current;

    if (num_cmds <= 0)
        return (1);
    i = 0;
    current = commands;
    while (i < num_cmds && current)
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            cleanup_resources(pipes, pids, num_cmds);
            return (1);
        }
        if (pids[i] == 0)
        {
            setup_child_fds(current, pipes, num_cmds, i);
            execute_child(current);
            exit(1);
        }
        close_pipe_ends(pipes, num_cmds, i);
        i++;
        current = current->next;
    }
    close_last_pipe(pipes, num_cmds);
    wait_for_children(pids, num_cmds);
    cleanup_resources(pipes, pids, num_cmds);
    return (0);
}

int count_commands(t_command *commands)
{
    int count = 0;              // Contatore comandi
    t_command *cmd = commands;  // Puntatore al primo comando
    
    while (cmd) {               // Finché ci sono comandi
        count++;                // Incrementa contatore
        cmd = cmd->next;        // Passa al prossimo
    }
    return count;               // Ritorna numero totale comandi
}

int **create_pipes(int num_cmds)
{
    int **pipes;                // Array di pipe
    int i;                      // Contatore

    if (num_cmds <= 1)         // Se c'è 0 o 1 comando, non servono pipe
        return NULL;
    
    pipes = malloc(sizeof(int *) * (num_cmds - 1));  // Alloca array di pipe
    if (!pipes)                 // Se malloc fallisce
        return NULL;
    
    for (i = 0; i < num_cmds - 1; ++i) {  // Per ogni pipe necessaria
        pipes[i] = malloc(sizeof(int) * 2);  // Alloca 2 fd per ogni pipe
        if (!pipes[i] || pipe(pipes[i]) < 0) {  // Se malloc o pipe falliscono
            perror("pipe");     // Stampa errore
            while (--i >= 0)    // Libera pipe già create
                free(pipes[i]);
            free(pipes);        // Libera array pipe
            return NULL;        // Esci con errore
        }
    }
    return pipes;               // Ritorna array pipe creato
}

void close_pipe_ends(int **pipes, int num_cmds, int current_cmd)
{
    if (num_cmds <= 1)
        return;
    
    // Chiudi tutte le pipe tranne quelle necessarie per il prossimo comando
    for (int i = 0; i < num_cmds - 1; i++) {
        if (i != current_cmd && i != current_cmd - 1) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
    }
    
    // Chiudi le pipe del comando corrente che non servono più
    if (current_cmd > 0) {
        close(pipes[current_cmd - 1][1]);  // Chiudi scrittura pipe precedente
    }
    if (current_cmd < num_cmds - 1) {
        close(pipes[current_cmd][0]);  // Chiudi lettura pipe corrente
    }
}

void setup_child_fds(t_command *cmd, int **pipes, int num_cmds, int i)
{
    // Prima chiudi tutti i file descriptor che non servono
    for (int j = 0; j < num_cmds - 1; ++j) {
        if (j == i - 1) {  // Pipe di input per il comando corrente
            if (i > 0)
                dup2(pipes[j][0], STDIN_FILENO);
        }
        else if (j == i) {  // Pipe di output per il comando corrente
            if (i < num_cmds - 1)
                dup2(pipes[j][1], STDOUT_FILENO);
        }
        close(pipes[j][0]);
        close(pipes[j][1]);
    }

    // Gestisci le redirezioni dopo le pipe
    if (cmd->in_fd >= 0)
        dup2(cmd->in_fd, STDIN_FILENO);
    if (cmd->out_fd >= 0)
        dup2(cmd->out_fd, STDOUT_FILENO);
}

void execute_child(t_command *cmd)
{
    if (cmd->is_builtin) {      // Se è un comando built-in
        execute_builtin(cmd);   // Eseguilo direttamente
        exit(g_state.last_status);  // Esci con status del built-in
    }

    // Se non abbiamo un percorso, prova a trovarlo in PATH
    if (!cmd->path && cmd->argv && cmd->argv[0]) {
        cmd->path = find_executable(cmd->argv[0]);
    }

    if (cmd->path) {  // Se abbiamo un percorso valido
        execve(cmd->path, cmd->argv, environ);  // Esegui comando esterno
        perror("execve");           // Se execve fallisce, stampa errore
    } else {
        ft_putstr_fd("minishell: command not found: ", 2);
        if (cmd->argv && cmd->argv[0])
            ft_putstr_fd(cmd->argv[0], 2);
        ft_putstr_fd("\n", 2);
    }
    exit(127);  // Esci con errore "command not found"
}

void wait_for_children(pid_t *pids, int num_cmds)
{
    for (int k = 0; k < num_cmds; ++k) {  // Per ogni processo figlio
        int status;             // Variabile per status
        waitpid(pids[k], &status, 0);  // Aspetta che il figlio termini
        if (WIFEXITED(status))  // Se il figlio è uscito normalmente
            g_state.last_status = WEXITSTATUS(status);  // Salva exit status
        else if (WIFSIGNALED(status))  // Se il figlio è stato terminato da un segnale
            g_state.last_status = 128 + WTERMSIG(status);  // Salva status segnale
    }
}

void cleanup_resources(int **pipes, pid_t *pids, int num_cmds)
{
    if (num_cmds > 1) {         // Se ci sono più di un comando
        for (int j = 0; j < num_cmds - 1; ++j)  // Per ogni pipe
            free(pipes[j]);     // Libera array pipe
        free(pipes);            // Libera array di array pipe
    }
    free(pids);                 // Libera array PID
}

int init_pipeline(t_command *commands, int ***pipes, pid_t **pids)
{
    int num_cmds = count_commands(commands);  // Conta numero comandi
    if (num_cmds == 0)          // Se non ci sono comandi
        return 0;               // Esci

    *pipes = create_pipes(num_cmds);  // Crea pipe necessarie
    if (num_cmds > 1 && !*pipes)  // Se servono pipe ma creazione fallisce
        return 0;               // Esci

    *pids = malloc(sizeof(pid_t) * num_cmds);  // Alloca array PID
    if (!*pids) {               // Se malloc fallisce
        cleanup_resources(*pipes, NULL, num_cmds);  // Libera risorse
        return 0;               // Esci
    }
    return num_cmds;            // Ritorna numero comandi
}

void close_last_pipe(int **pipes, int num_cmds)
{
    if (num_cmds > 1) {         // Se ci sono più di un comando
        close(pipes[num_cmds - 2][0]);  // Chiudi fd lettura ultima pipe
        close(pipes[num_cmds - 2][1]);  // Chiudi fd scrittura ultima pipe
    }
}

/*
  Pulisce tutte le risorse dopo l'esecuzione dei comandi.
  commands: lista dei comandi da pulire
  La funzione si assicura che tutti i file descriptor siano chiusi
  e che tutta la memoria allocata sia liberata.
*/
void cleanup_after_execution(t_command *commands)
{
    t_command *current;
    t_command *next;

    current = commands;
    while (current) {
        // Salva il prossimo comando prima di liberare il corrente
        next = current->next;

        // Chiudi i file descriptor se sono aperti
        if (current->in_fd >= 0) {
            close(current->in_fd);
            current->in_fd = -1;
        }
        if (current->out_fd >= 0) {
            close(current->out_fd);
            current->out_fd = -1;
        }

        // Libera l'array degli argomenti
        if (current->argv) {
            for (int i = 0; current->argv[i]; i++)
                free(current->argv[i]);
            free(current->argv);
        }

        // Libera il percorso del comando
        if (current->path)
            free(current->path);

        // Libera il comando corrente
        free(current);

        // Passa al prossimo comando
        current = next;
    }
}
