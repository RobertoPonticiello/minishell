#include "libft.h" // Per ft_putendl_fd
#include <unistd.h>
#include <string.h> // Per strncmp

extern char **environ;

// --- CHECCO: inizio funzione ft_env ---
// Stampa tutte le variabili d'ambiente usando la funzione della libft
int ft_env(char **args)
{
    (void)args;
    int i = 0; // Indice per scorrere l'array
    while (environ && environ[i]) { // Finché ci sono variabili
        // Salta COLUMNS e LINES che sono aggiunte da readline
        if (strncmp(environ[i], "COLUMNS=", 8) != 0 && 
            strncmp(environ[i], "LINES=", 6) != 0) {
            ft_putendl_fd(environ[i], 1); // Stampa la variabile su stdout
        }
        i++;
    }
    return (0);
}
// --- CHECCO: fine funzione ft_env ---
