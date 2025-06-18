/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffebbrar <ffebbrar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 19:24:01 by ffebbrar          #+#    #+#             */
/*   Updated: 2025/06/11 19:40:07 by ffebbrar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Controlla gli errori di sintassi nella lista di token.
** 
** @param tokens: La lista di token da controllare
** @return: 1 se viene trovato un errore di sintassi, 0 altrimenti
**
** La funzione verifica la correttezza sintattica della lista di token,
** controllando:
** - Non ci sono pipe all'inizio della lista
** - Non ci sono pipe consecutive
** - Ogni redirezione è seguita da una parola
** Se viene trovato un errore, la funzione restituisce 1.
*/
int check_syntax_errors(t_token *tokens)
{
    t_token *curr;

    if (!tokens)
        return (0);
    if (tokens->type == TOKEN_PIPE)
        return (1);
    curr = tokens;
    while (curr)
    {
        if (curr->type == TOKEN_PIPE)
        {
            if (!curr->next || curr->next->type == TOKEN_PIPE)
                return (1);
        }
        if (curr->type == TOKEN_REDIR_IN || curr->type == TOKEN_REDIR_OUT ||
            curr->type == TOKEN_HEREDOC || curr->type == TOKEN_APPEND)
        {
            if (!curr->next || curr->next->type != TOKEN_WORD)
                return (1);
        }
        curr = curr->next;
    }
    return (0);
}
