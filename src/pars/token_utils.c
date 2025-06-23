/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffebbrar <ffebbrar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 19:24:01 by ffebbrar          #+#    #+#             */
/*   Updated: 2025/06/17 17:35:36 by ffebbrar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Crea un nuovo token e lo aggiunge in coda alla lista.
** 
** @param val: Il valore del token
** @param type: Il tipo del token
** @return: Il nuovo token creato, o NULL in caso di errore
**
** La funzione alloca memoria per un nuovo token, copia il valore
** e imposta il tipo. Il token viene inizializzato con next = NULL.
** In caso di errore di allocazione, restituisce NULL.
*/
t_token *create_token(const char *val, t_token_type type)
{
    t_token *tok;

    tok = malloc(sizeof(t_token));
    if (!tok)
        return (NULL);
    tok->value = strdup(val);
    tok->type = type;
    tok->next = NULL;
    return (tok);
}

/*
** Gestisce gli operatori di redirezione (<, >, <<, >>).
** 
** @param line: La stringa da processare
** @param i: Indice corrente nella stringa
** @param tok: Puntatore al token da creare
** @return: La lunghezza dell'operatore riconosciuto (1 o 2)
**
** La funzione riconosce gli operatori di redirezione e crea
** il token appropriato. Restituisce la lunghezza dell'operatore
** per permettere l'avanzamento corretto nell'input.
*/
t_token *handle_redirection_operator(const char *input, int *i)
{
    if (input[*i] == '<' && input[*i + 1] == '<')
    {
        t_token *tok = create_token("<<", TOKEN_HEREDOC);
        *i += 2;
        return (tok);
    }
    if (input[*i] == '>' && input[*i + 1] == '>')
    {
        t_token *tok = create_token(">>", TOKEN_APPEND);
        *i += 2;
        return (tok);
    }
    if (input[*i] == '<')
    {
        t_token *tok = create_token("<", TOKEN_REDIR_IN);
        *i += 1;
        return (tok);
    }
    if (input[*i] == '>')
    {
        t_token *tok = create_token(">", TOKEN_REDIR_OUT);
        *i += 1;
        return (tok);
    }
    if (input[*i] == '|')
    {
        t_token *tok = create_token("|", TOKEN_PIPE);
        *i += 1;
        return (tok);
    }
    return (NULL);
}

/*
** Riconosce e gestisce operatori speciali (|, <, >, <<, >>).
** 
** @param line: La stringa da processare
** @param i: Indice corrente nella stringa
** @param tok: Puntatore al token da creare
** @return: La lunghezza dell'operatore riconosciuto (1 o 2)
**
** La funzione riconosce tutti gli operatori speciali della shell:
** - | (pipe)
** - < (input redirection)
** - > (output redirection)
** - << (heredoc)
** - >> (append)
** Restituisce la lunghezza dell'operatore per l'avanzamento.
*/
int is_operator(const char *input, int i)
{
    if (input[i] == '|' || input[i] == '<' || input[i] == '>')
        return (1);
    return (0);
}

/*
** Estrae una parola tra virgolette.
** 
** @param line: La stringa da processare
** @param i: Indice corrente nella stringa
** @param out: Puntatore alla stringa estratta
** @return: La lunghezza della parola estratta
**
** La funzione estrae il contenuto tra virgolette singole o doppie,
** copiandolo nella stringa di output. Le virgolette vengono rimosse
** dal risultato. La memoria per la stringa estratta viene allocata
** dinamicamente.
*/
char *extract_quoted_word(const char *input, int *i, char quote)
{
    int start;
    int len;

    start = *i + 1; // Salta la virgoletta iniziale
    len = 0;
    (*i)++; // Avanza oltre la virgoletta iniziale
    while (input[*i] && input[*i] != quote) {
        (*i)++;
        len++;
    }
    char *out = strndup(input + start, len); // Copia solo il contenuto
    if (input[*i] == quote)
        (*i)++; // Salta la virgoletta finale
    return (out);
}

/*
** Estrae una parola o una stringa tra virgolette.
** 
** @param line: La stringa da processare
** @param i: Indice corrente nella stringa
** @param out: Puntatore alla stringa estratta
** @return: La lunghezza della parola estratta
**
** La funzione estrae una parola dalla stringa, gestendo:
** - Parole tra virgolette singole o doppie
** - Parole normali (fino al prossimo spazio o operatore)
** La memoria per la stringa estratta viene allocata dinamicamente.
*/
char *extract_word(const char *input, int *i)
{
    int start;
    int len;

    start = *i;
    len = 0;
    if (input[*i] == '\'' || input[*i] == '"')
        return (extract_quoted_word(input, i, input[*i]));
    while (input[*i] && input[*i] != ' ' && input[*i] != '\t' &&
           input[*i] != '|' && input[*i] != '<' && input[*i] != '>' &&
           input[*i] != '\'' && input[*i] != '"')
        *i += 1;
    len = *i - start;
    char *out = strndup(input + start, len);
    return (out);
}
