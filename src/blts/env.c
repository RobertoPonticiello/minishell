#include "libft.h" // Per ft_putendl_fd

// --- CHECCO: inizio funzione ft_env ---
// Stampa tutte le variabili d'ambiente usando la funzione della libft
int ft_env(char **envp)
{
    int i = 0; // Indice per scorrere l'array
    while (envp && envp[i]) { // Finché ci sono variabili
        ft_putendl_fd(envp[i], 1); // Stampa la variabile su stdout
        i++;
    }
    return 0; // Successo
}
// --- CHECCO: fine funzione ft_env ---
