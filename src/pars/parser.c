#include "minishell.h"

/*
** Funzione principale di parsing.
** 
** @param line: La stringa da parsare
** @return: La lista di comandi generata, o NULL in caso di errore
**
** La funzione esegue il parsing completo dell'input:
** 1. Tokenizza la stringa
** 2. Gestisce le virgolette
** 3. Espande le variabili
** 4. Controlla la sintassi
** 5. Costruisce i comandi
*/
t_command *parse_input(const char *line)
{
    t_token *tokens;
    t_command *commands;

    if (!line || !*line)
        return (NULL);

    tokens = tokenize(line); /* 1. Tokenizzazione */
    if (!tokens)
        return (NULL);

    handle_quotes(tokens); /* 2. Gestione virgolette ed espansione variabili */
    expand_variables(&tokens); /* 3. Rimozione token vuoti dopo espansione */

    if (check_syntax_errors(tokens)) /* 4. Controllo sintassi */
    {
        free_tokens(tokens);
        return (NULL);
    }

    commands = build_commands(tokens); /* 5. Costruzione comandi */
    free_tokens(tokens);

    return (commands);
}
