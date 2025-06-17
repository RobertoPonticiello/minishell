/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffebbrar <ffebbrar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 19:24:01 by ffebbrar          #+#    #+#             */
/*   Updated: 2025/06/11 19:40:05 by ffebbrar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Libera tutta la memoria allocata dalla lista di comandi
void free_commands(t_command *cmds)
{
    t_command *curr;
    t_command *next;

    curr = cmds;
    while (curr)
    {
        next = curr->next;
        if (curr->argv)
        {
            for (int i = 0; curr->argv[i]; i++)
                free(curr->argv[i]);
            free(curr->argv);
        }
        if (curr->path)
            free(curr->path);
        free(curr);
        curr = next;
    }
}

/*
** Libera la lista di token.
** 
** @param tokens: La lista di token da liberare
**
** La funzione libera tutta la memoria allocata per una lista di token:
** - Il valore di ogni token (se presente)
** - Il token stesso
** La funzione gestisce correttamente la lista collegata, mantenendo
** un puntatore al prossimo token prima di liberare quello corrente.
*/
void free_tokens(t_token *tokens)
{
    t_token *curr;
    t_token *next;

    curr = tokens;
    while (curr)
    {
        next = curr->next;
        if (curr->value)
            free(curr->value);
        free(curr);
        curr = next;
    }
}
