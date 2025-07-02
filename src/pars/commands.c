/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffebbrar <ffebbrar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 19:24:01 by ffebbrar          #+#    #+#             */
/*   Updated: 2025/06/17 17:32:28 by ffebbrar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Verifica se un comando è built-in.
** 
** @param cmd_name: Il nome del comando da verificare
** @return: 1 se è built-in, 0 altrimenti
*/
static int is_builtin_command(const char *cmd_name)
{
    const char *builtins[] = {
        "echo", "cd", "pwd", "export", "unset", "env", "exit", NULL
    };
    int i;

    i = 0;
    while (builtins[i])
    {
        if (strcmp(cmd_name, builtins[i]) == 0)
            return (1);
        i++;
    }
    return (0);
}

/*
** Inizializza un nuovo comando con i suoi argomenti.
** 
** @param curr: Il token corrente da cui iniziare l'estrazione
** @param argc: Puntatore al contatore degli argomenti
** @param argv: Array dove salvare gli argomenti
** @return: Il comando inizializzato
**
** La funzione:
** - Alloca memoria per il comando
** - Inizializza i file descriptor a -1
** - Estrae gli argomenti dai token fino al prossimo pipe
** - Gestisce le redirezioni
** - Costruisce l'array degli argomenti
*/
static t_command *init_command(t_token *curr, int *argc, char **argv)
{
    t_command *cmd;

    cmd = calloc(1, sizeof(t_command));
    cmd->in_fd = -1;
    cmd->out_fd = -1;
    cmd->redir_error = 0;
    *argc = 0;
    while (curr && curr->type != TOKEN_PIPE)
    {
        if (curr->type == TOKEN_WORD)
            argv[(*argc)++] = strdup(curr->value);
        else if (curr->type >= TOKEN_REDIR_IN && curr->type <= TOKEN_APPEND)
        {
            if (handle_redirection(cmd, curr) == -1)
            {
                cmd->redir_error = 1;
                // Se una redirezione fallisce, fermarsi qui - bash non continua
                break;
            }
            // Salta il token della redirezione e il file che segue
            curr = curr->next; // Salta il nome del file
            if (curr) 
                curr = curr->next; // Va al token successivo
            continue; // Non fare curr = curr->next alla fine del loop
        }
        curr = curr->next;
    }
    argv[*argc] = NULL;
    cmd->argv = malloc((*argc + 1) * sizeof(char*));
    for (int i = 0; i < *argc; i++)
        cmd->argv[i] = argv[i];  // Trasferisce ownership, non copia
    cmd->argv[*argc] = NULL;
    
    // Se non ci sono argomenti e non ci sono errori di redirezione, libera e restituisci NULL
    if (*argc == 0 && !cmd->redir_error) {
        free(cmd->argv);
        free(cmd);
        return NULL;
    }
    
    if (cmd->argv[0])  // Se c'è un comando
        cmd->is_builtin = is_builtin_command(cmd->argv[0]);  // Verifica se è built-in
    return (cmd);
}

/*
** Costruisce una lista di comandi (pipeline) dai token.
** 
** @param tokens: La lista di token da cui costruire i comandi
** @return: La testa della lista dei comandi
**
** La funzione:
** - Crea una lista di comandi separati da pipe
** - Per ogni comando:
**   - Inizializza il comando con i suoi argomenti
**   - Gestisce le redirezioni
**   - Collega i comandi in pipeline
*/
t_command *build_commands(t_token *tokens)
{
    t_command *head;
    t_command *tail;
    t_token *curr;
    t_command *cmd;
    int argc;
    char *argv[256];

    head = NULL;
    tail = NULL;
    curr = tokens;
    while (curr)
    {
        cmd = init_command(curr, &argc, argv);
        
        // Solo aggiungi il comando se non è NULL
        if (cmd) {
            if (!head)
                head = cmd;
            else
                tail->next = cmd;
            tail = cmd;
        }
        
        // Trova il prossimo pipe o fine lista
        while (curr && curr->type != TOKEN_PIPE)
        {
            if (curr->type >= TOKEN_REDIR_IN && curr->type <= TOKEN_APPEND)
            {
                curr = curr->next; // Salta il token redirezione
                if (curr)
                    curr = curr->next; // Salta il nome del file
            }
            else
                curr = curr->next;
        }
        if (curr && curr->type == TOKEN_PIPE)
            curr = curr->next;
    }
    return (head);
}
