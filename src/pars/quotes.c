/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffebbrar <ffebbrar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 19:24:01 by ffebbrar          #+#    #+#             */
/*   Updated: 2025/06/17 17:34:51 by ffebbrar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Gestisce le virgolette in un token.
** 
** @param token: Il token da processare
*/
void handle_quotes(t_token *token)
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
                if (token->value[i] == '$' && token->value[i + 1])
                    copy_env_value(token->value, &i, result, &j);
                else
                    result[j++] = token->value[i++];
            }
            if (token->value[i] == '"')
                i++;  // Salta la virgoletta di chiusura
        }
        else if (!in_single_quotes && token->value[i] == '$' && token->value[i + 1])
            copy_env_value(token->value, &i, result, &j);
        else
            result[j++] = token->value[i++];
    }
    result[j] = 0;
    free(token->value);
    token->value = strdup(result);
    free(result);
}
