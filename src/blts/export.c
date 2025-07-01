#include "minishell.h" // Per le dichiarazioni delle funzioni
#include "libft.h" // Per ft_putstr_fd, ft_putendl_fd
#include <stdlib.h>   // Per setenv
#include <string.h>   // Per strchr
#include <ctype.h>    // Per isalpha, isalnum

// Controlla se un nome è un identificatore valido per export
static int is_valid_identifier(const char *name)
{
    int i = 0;
    
    if (!name || !name[0])
        return 0;
    
    // Deve iniziare con lettera o underscore
    if (!isalpha(name[0]) && name[0] != '_')
        return 0;
    
    // Il resto può contenere solo lettere, numeri e underscore
    i = 1;
    while (name[i])
    {
        if (!isalnum(name[i]) && name[i] != '_')
            return 0;
        i++;
    }
    return 1;
}

// --- CHECCO: inizio funzione ft_export ---
// Aggiunge o aggiorna variabili d'ambiente, stampa errore se nome non valido
int ft_export(char **args)
{
    int i = 1;
    int status = 0;
    
    if (!args[1]) // Se non ci sono argomenti, stampa tutte le variabili
        return ft_env(args);
    
    while (args[i])
    {
        char *eq = strchr(args[i], '=');
        
        if (eq) // Formato VAR=value
        {
            *eq = '\0'; // Divide temporaneamente nome e valore
            if (!is_valid_identifier(args[i]))
            {
                ft_putstr_fd("export: not a valid identifier: ", 2);
                ft_putendl_fd(args[i], 2);
                status = 1;
            }
            else
            {
                setenv(args[i], eq + 1, 1); // Aggiorna la variabile
            }
            *eq = '='; // Ripristina la stringa
        }
        else // Formato VAR (senza valore)
        {
            if (!is_valid_identifier(args[i]))
            {
                ft_putstr_fd("export: not a valid identifier: ", 2);
                ft_putendl_fd(args[i], 2);
                status = 1;
            }
            // Per export senza valore, bash lo considera valido ma non fa nulla
            // Non aggiungiamo la variabile all'environment se non ha valore
        }
        i++;
    }
    return status;
}
// --- CHECCO: fine funzione ft_export ---
