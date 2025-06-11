#include "libft.h" // Per ft_putstr_fd, ft_putendl_fd
#include <stdlib.h>   // Per setenv
#include <string.h>   // Per strchr

// --- CHECCO: inizio funzione ft_export ---
// Aggiunge o aggiorna variabili d'ambiente, stampa errore se nome non valido
int ft_export(char **args, char **envp)
{
    int i = 1; // Inizia dal primo argomento dopo il comando
    if (!args[1]) // Se non ci sono argomenti, stampa tutte le variabili
        return ft_env(envp);
    while (args[i]) {
        char *eq = strchr(args[i], '='); // Cerca il simbolo =
        if (!eq || eq == args[i]) { // Nome non valido
            ft_putstr_fd("export: not a valid identifier: ", 2); // Stampa errore su stderr
            ft_putendl_fd(args[i], 2);
            return 1;
        }
        *eq = 0; // Divide nome e valore
        setenv(args[i], eq + 1, 1); // Aggiorna o aggiunge la variabile
        *eq = '='; // Ripristina la stringa
        i++;
    }
    return 0; // Successo
}
// --- CHECCO: fine funzione ft_export ---
