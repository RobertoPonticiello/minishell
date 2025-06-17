#include "libft.h" // Per ft_putendl_fd
#include <unistd.h>

extern char **environ;

// --- CHECCO: inizio funzione ft_env ---
// Stampa tutte le variabili d'ambiente usando la funzione della libft
int ft_env(char **args)
{
    (void)args;
    int i = 0; // Indice per scorrere l'array
    while (environ && environ[i]) { // Finché ci sono variabili
        ft_putendl_fd(environ[i], 1); // Stampa la variabile su stdout
        i++;
    }
    return (0);
}
// --- CHECCO: fine funzione ft_env ---
