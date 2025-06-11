#include "minishell.h"  // contiene t_shell_state g_state e prototipi

/*
  Esegue una lista di comandi collegati da pipe e redirezioni.
  commands è la testa di una lista collegata (linked list) di t_command.
*/
void execute_pipeline(t_command *commands)
{
    int **pipes = NULL;        // Array di pipe, ogni pipe ha 2 fd [read, write]
    pid_t *pids = NULL;        // Array per tenere traccia dei PID dei processi figli
    int num_cmds = init_pipeline(commands, &pipes, &pids);  // Inizializza pipe e pids, ritorna numero comandi
    
    if (!num_cmds)             // Se non ci sono comandi, esci
        return;

    t_command *cmd = commands; // Puntatore al primo comando
    for (int i = 0; cmd; i++) {  // Per ogni comando nella lista
        pid_t pid = fork();     // Crea un nuovo processo
        if (pid < 0) {          // Se fork fallisce
            perror("fork");     // Stampa errore
            cleanup_resources(pipes, pids, num_cmds);  // Libera risorse
            return;             // Esci
        }

        if (pid == 0) {         // Se siamo nel processo figlio
            setup_child_fds(cmd, pipes, num_cmds, i);  // Configura file descriptor
            execute_child(cmd);  // Esegui il comando
        }

        pids[i] = pid;          // Salva il PID del figlio
        close_pipe_ends(pipes, num_cmds, i);  // Chiudi le pipe non più necessarie
        cmd = cmd->next;        // Passa al prossimo comando
    }

    close_last_pipe(pipes, num_cmds);  // Chiudi l'ultima pipe rimasta aperta
    wait_for_children(pids, num_cmds); // Aspetta che tutti i figli terminino
    cleanup_resources(pipes, pids, num_cmds);  // Libera tutta la memoria
}

static int count_commands(t_command *commands)
{
    int count = 0;              // Contatore comandi
    t_command *cmd = commands;  // Puntatore al primo comando
    
    while (cmd) {               // Finché ci sono comandi
        count++;                // Incrementa contatore
        cmd = cmd->next;        // Passa al prossimo
    }
    return count;               // Ritorna numero totale comandi
}

static int **create_pipes(int num_cmds)
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

static void close_pipe_ends(int **pipes, int num_cmds, int current_cmd)
{
    if (current_cmd > 0) {      // Se non è il primo comando
        close(pipes[current_cmd - 1][0]);  // Chiudi fd lettura pipe precedente
        close(pipes[current_cmd - 1][1]);  // Chiudi fd scrittura pipe precedente
    }
}

static void setup_child_fds(t_command *cmd, int **pipes, int num_cmds, int i)
{
    if (i > 0)                  // Se non è il primo comando
        dup2(pipes[i - 1][0], STDIN_FILENO);  // Leggi dalla pipe precedente
    if (i < num_cmds - 1)       // Se non è l'ultimo comando
        dup2(pipes[i][1], STDOUT_FILENO);     // Scrivi sulla pipe successiva
    
    if (cmd->in_fd >= 0)        // Se c'è redirezione input
        dup2(cmd->in_fd, STDIN_FILENO);       // Usa file come input
    if (cmd->out_fd >= 0)       // Se c'è redirezione output
        dup2(cmd->out_fd, STDOUT_FILENO);     // Usa file come output
    
    for (int j = 0; j < num_cmds - 1; ++j) {  // Per ogni pipe
        close(pipes[j][0]);     // Chiudi fd lettura
        close(pipes[j][1]);     // Chiudi fd scrittura
    }
}

static void execute_child(t_command *cmd)
{
    if (cmd->is_builtin) {      // Se è un comando built-in
        execute_builtin(cmd);   // Eseguilo direttamente
        exit(g_state.last_status);  // Esci con status del built-in
    }
    execve(cmd->path, cmd->argv, environ);  // Altrimenti esegui comando esterno
    perror("execve");           // Se execve fallisce, stampa errore
    exit(EXIT_FAILURE);         // Esci con errore
}

static void wait_for_children(pid_t *pids, int num_cmds)
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

static void cleanup_resources(int **pipes, pid_t *pids, int num_cmds)
{
    if (num_cmds > 1) {         // Se ci sono più di un comando
        for (int j = 0; j < num_cmds - 1; ++j)  // Per ogni pipe
            free(pipes[j]);     // Libera array pipe
        free(pipes);            // Libera array di array pipe
    }
    free(pids);                 // Libera array PID
}

static int init_pipeline(t_command *commands, int **pipes, pid_t **pids)
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

static void close_last_pipe(int **pipes, int num_cmds)
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
