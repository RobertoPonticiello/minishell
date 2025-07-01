/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffebbrar <ffebbrar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 19:24:01 by ffebbrar          #+#    #+#             */
/*   Updated: 2025/06/30 20:27:17 by ffebbrar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Gestisce le virgolette in un singolo token.
** 
** @param token: Il token da processare
*/
static void handle_single_token_quotes(t_token *token)
{
    char *result;
    int i;
    int j;
    int in_single_quotes = 0;  // Flag per tracciare se siamo dentro single quotes

    if (!token || !token->value || token->type != TOKEN_WORD)
        return;
    
    result = malloc(4096);
    if (!result)
        return;
    i = 0;
    j = 0;
    while (token->value[i])
    {
        if (token->value[i] == '\'')
        {
            in_single_quotes = !in_single_quotes;  // Toggle del flag
            i++;  // Salta la virgoletta
            continue;
        }
        else if (token->value[i] == '"' && !in_single_quotes)
        {
            i++;  // Salta la virgoletta
            while (token->value[i] && token->value[i] != '"')
            {
                // Gestisce i caratteri di escape nelle double quotes
                if (token->value[i] == '\\' && token->value[i + 1])
                {
                    process_escape(token->value, &i, result, &j);
                }
                // Nelle double quotes, espandi le variabili
                else if (token->value[i] == '$' && token->value[i + 1])
                    copy_env_value(token->value, &i, result, &j);
                else
                    result[j++] = token->value[i++];
            }
            if (token->value[i] == '"')
                i++;  // Salta la virgoletta di chiusura
        }
        else if (!in_single_quotes && token->value[i] == '$' && token->value[i + 1])
        {
            // Fuori dalle virgolette, espandi le variabili
            copy_env_value(token->value, &i, result, &j);
        }
        else
            result[j++] = token->value[i++];
    }
    result[j] = 0;
    
    free(token->value);
    token->value = strdup(result);
    free(result);
}

/*
** Gestisce le virgolette in tutti i token della lista.
** 
** @param tokens: Lista di token da processare
*/
void handle_quotes(t_token *tokens)
{
    t_token *current = tokens;
    
    while (current)
    {
        if (current->type == TOKEN_WORD)
            handle_single_token_quotes(current);
        current = current->next;
    }
}
