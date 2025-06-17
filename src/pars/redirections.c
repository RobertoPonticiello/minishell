/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffebbrar <ffebbrar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 19:24:01 by ffebbrar          #+#    #+#             */
/*   Updated: 2025/06/11 19:40:10 by ffebbrar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <fcntl.h>  // Per i flag O_RDONLY, O_WRONLY, O_CREAT, O_APPEND, O_TRUNC

/*
** Gestisce le redirezioni di input (< e <<).
** 
** @param cmd: Il comando a cui applicare la redirezione
** @param curr: Il token di redirezione corrente
**
** La funzione gestisce le redirezioni di input:
** - Chiude il file descriptor di input esistente se presente
** - Apre il nuovo file in lettura
** - Gestisce gli errori di apertura e chiusura
** - Aggiorna il file descriptor di input del comando
*/
static void handle_input_redirection(t_command *cmd, t_token *curr)
{
    int fd;

    if (cmd->in_fd > 0)
    {
        if (close(cmd->in_fd) == -1)
            perror("minishell: close");
    }
    fd = open(curr->next->value, O_RDONLY);
    if (fd == -1)
    {
        perror("minishell");
        g_state.last_status = 1;
        return;
    }
    cmd->in_fd = fd;
}

/*
** Gestisce le redirezioni di output (> e >>).
** 
** @param cmd: Il comando a cui applicare la redirezione
** @param curr: Il token di redirezione corrente
**
** La funzione gestisce le redirezioni di output:
** - Chiude il file descriptor di output esistente se presente
** - Apre il nuovo file in scrittura con i flag appropriati
** - Gestisce gli errori di apertura e chiusura
** - Aggiorna il file descriptor di output del comando
*/
static void handle_output_redirection(t_command *cmd, t_token *curr)
{
    int flags;
    int fd;

    if (cmd->out_fd > 0)
    {
        if (close(cmd->out_fd) == -1)
            perror("minishell: close");
    }
    flags = O_WRONLY | O_CREAT;
    if (curr->type == TOKEN_APPEND)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;
    fd = open(curr->next->value, flags, 0644);
    if (fd == -1)
    {
        perror("minishell");
        g_state.last_status = 1;
        return;
    }
    cmd->out_fd = fd;
}

/*
** Gestisce le redirezioni e aggiorna i file descriptor del comando.
** 
** @param cmd: Il comando a cui applicare le redirezioni
** @param curr: Il token di redirezione corrente
**
** La funzione gestisce tutti i tipi di redirezione:
** - < (input)
** - > (output)
** - << (heredoc)
** - >> (append)
** Chiama le funzioni appropriate in base al tipo di redirezione.
*/
void handle_redirection(t_command *cmd, t_token *curr)
{
    if ((curr->type == TOKEN_REDIR_IN || 
         curr->type == TOKEN_HEREDOC) && curr->next)
        handle_input_redirection(cmd, curr);
    if ((curr->type == TOKEN_REDIR_OUT || 
         curr->type == TOKEN_APPEND) && curr->next)
        handle_output_redirection(cmd, curr);
}
