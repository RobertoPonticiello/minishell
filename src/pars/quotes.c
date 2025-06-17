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
** Gestisce una stringa tra virgolette singole (').
** 
** @param str: La stringa da processare
** @param i: Puntatore all'indice corrente nella stringa
** @param result: Buffer dove salvare il risultato
** @param j: Puntatore all'indice corrente nel buffer risultato
** @return: 1 se la gestione è avvenuta con successo
*/
static int handle_single_quotes(const char *str, int *i, char *result, int *j)
{
    (*i)++;  // Salta la virgoletta iniziale
    while (str[*i] && str[*i] != '\'')
        result[(*j)++] = str[(*i)++];  // Copia solo il contenuto
    if (str[*i] == '\'')
        (*i)++;  // Salta la virgoletta finale
    return (1);
}

/*
** Gestisce una stringa tra virgolette doppie (").
** 
** @param str: La stringa da processare
** @param i: Puntatore all'indice corrente nella stringa
** @param result: Buffer dove salvare il risultato
** @param j: Puntatore all'indice corrente nel buffer risultato
** @return: 1 se la gestione è avvenuta con successo
*/
static int handle_double_quotes(const char *str, int *i, char *result, int *j)
{
    (*i)++;  // Salta la virgoletta iniziale
    while (str[*i] && str[*i] != '"')
    {
        if (str[*i] == '\\' && str[*i + 1])
        {
            if (process_escape(str, i, result, j))
                continue;
        }
        if (str[*i] == '$' && str[*i + 1])
        {
            if (copy_env_value(str, i, result, j))
                continue;
        }
        result[(*j)++] = str[(*i)++];  // Copia solo il contenuto
    }
    if (str[*i] == '"')
        (*i)++;  // Salta la virgoletta finale
    return (1);
}

/*
** Processa un carattere normale (non tra virgolette).
** 
** @param str: La stringa da processare
** @param i: Puntatore all'indice corrente nella stringa
** @param result: Buffer dove salvare il risultato
** @param j: Puntatore all'indice corrente nel buffer risultato
** @return: 1 se la gestione è avvenuta con successo
*/
static int handle_normal_char(const char *str, int *i, char *result, int *j)
{
    if (str[*i] == '\\' && str[*i + 1])
    {
        if (process_escape(str, i, result, j))
            return (1);
    }
    if (str[*i] == '$' && str[*i + 1])
    {
        if (copy_env_value(str, i, result, j))
            return (1);
    }
    result[(*j)++] = str[(*i)++];
    return (1);
}

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
            handle_single_quotes(token->value, &i, result, &j);
        else if (token->value[i] == '"')
            handle_double_quotes(token->value, &i, result, &j);
        else
            handle_normal_char(token->value, &i, result, &j);
    }
    result[j] = 0;
    free(token->value);
    token->value = strdup(result);
    free(result);
}
