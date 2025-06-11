#include <stdlib.h>   // Per unsetenv
#include "libft.h"   // Per ft_putstr_fd, ft_putendl_fd

// --- CHECCO: inizio funzione ft_unset ---
// Rimuove variabili d'ambiente, stampa errore se nome non valido
int ft_unset(char **args)
{
    int i = 1; // Inizia dal primo argomento dopo il comando
    while (args[i]) {
        if (!args[i][0]) { // Nome non valido
            ft_putstr_fd("unset: not a valid identifier: ", 2);
            ft_putendl_fd(args[i], 2);
            return 1;
        }
        unsetenv(args[i]); // Rimuove la variabile d'ambiente
        i++;
    }
    return 0; // Successo
}
// --- CHECCO: fine funzione ft_unset ---
