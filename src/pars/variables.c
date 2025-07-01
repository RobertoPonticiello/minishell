/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   variables.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ffebbrar <ffebbrar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 19:33:02 by ffebbrar          #+#    #+#             */
/*   Updated: 2025/06/30 20:48:42 by ffebbrar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <ctype.h>  // Per isalnum

/*
** Gestisce l'espansione della variabile $? (stato di uscita).
** 
** @param dst: Buffer dove salvare il risultato
** @param di: Puntatore all'indice corrente nel buffer risultato
** @return: 1 se l'espansione è avvenuta con successo
**
** La funzione converte lo stato di uscita dell'ultimo comando (g_state.last_status)
** in una stringa e la copia nel buffer risultato.
*/
static int handle_exit_status(char *dst, int *di)
{
    char status[12];
    int k;

    snprintf(status, sizeof(status), "%d", g_state.last_status);
    k = 0;
    while (status[k])
        dst[(*di)++] = status[k++];
    return (1);
}

/*
** Gestisce l'espansione di una variabile d'ambiente.
** 
** @param src: La stringa sorgente contenente la variabile
** @param var_start: Indice di inizio del nome della variabile
** @param var_len: Lunghezza del nome della variabile
** @param dst: Buffer dove salvare il risultato
** @param di: Puntatore all'indice corrente nel buffer risultato
** @param si: Puntatore all'indice corrente nella stringa sorgente
** @return: 1 se l'espansione è avvenuta con successo
**
** La funzione estrae il nome della variabile, ottiene il suo valore usando getenv
** e lo copia nel buffer risultato. Se la variabile non esiste, non viene copiato
** nulla nel risultato.
*/
static int handle_env_var(const char *src, int var_start, int var_len, 
                         char *dst, int *di, int *si)
{
    char var[256];
    char *val;

    strncpy(var, src + var_start, var_len);
    var[var_len] = 0;
    val = getenv(var);
    if (val)
    {
        for (int k = 0; val[k]; k++)
            dst[(*di)++] = val[k];
    }
    *si = var_start + var_len;
    return (1);
}

/*
** Espande $VAR e $? in una stringa.
** 
** @param src: La stringa sorgente
** @param si: Puntatore all'indice corrente nella stringa sorgente
** @param dst: Buffer dove salvare il risultato
** @param di: Puntatore all'indice corrente nel buffer risultato
** @return: 1 se l'espansione è avvenuta con successo, 0 altrimenti
**
** La funzione gestisce sia l'espansione di $? (stato di uscita) che di $VAR
** (variabili d'ambiente). Se non viene trovata una variabile valida, la funzione
** restituisce 0 e non modifica gli indici.
*/
int copy_env_value(const char *src, int *si, char *dst, int *di)
{
    int var_start;
    int var_len;

    if (src[*si + 1] == '?')
    {
        *si += 2;
        return (handle_exit_status(dst, di));
    }
    var_start = *si + 1;
    var_len = 0;
    while (src[var_start + var_len] && 
           (isalnum(src[var_start + var_len]) || 
            src[var_start + var_len] == '_'))
        var_len++;
    if (var_len > 0)
        return (handle_env_var(src, var_start, var_len, dst, di, si));
    
    // Se non c'è una variabile valida dopo $, tratta $ come carattere normale
    dst[(*di)++] = src[(*si)++];
    return (1);
}

/*
** Espande le variabili in una stringa.
** 
** @param src: La stringa sorgente
** @return: Una nuova stringa con le variabili espanse
**
** La funzione crea un nuovo buffer e copia la stringa sorgente, espandendo
** tutte le variabili d'ambiente ($VAR) e lo stato di uscita ($?) che trova.
** La memoria per il risultato viene allocata dinamicamente e deve essere
** liberata dal chiamante.
*/
char *expand_string(const char *src)
{
    char *dst;
    int si;
    int di;

    dst = malloc(4096);
    si = 0;
    di = 0;
    while (src[si])
    {
        if (src[si] == '$' && src[si + 1])
        {
            if (copy_env_value(src, &si, dst, &di))
                continue;
        }
        dst[di++] = src[si++];
    }
    dst[di] = 0;
    return (dst);
}

/*
** Espande le variabili in tutti i token.
** 
** @param tokens: Puntatore alla lista di token da processare
**
** La funzione itera su tutti i token di tipo TOKEN_WORD e espande le variabili
** d'ambiente e lo stato di uscita nel loro valore. La memoria viene gestita
** correttamente, liberando i vecchi valori e allocando i nuovi.
*/
void expand_variables(t_token **tokens)
{
    // L'espansione è già fatta da handle_quotes
    // Qui rimuoviamo solo i token vuoti all'inizio di ogni comando (fino al primo pipe)
    while (*tokens && (*tokens)->type == TOKEN_WORD && (*tokens)->value && strlen((*tokens)->value) == 0)
    {
        t_token *to_remove = *tokens;
        *tokens = (*tokens)->next;
        free(to_remove->value);
        free(to_remove);
    }
}
