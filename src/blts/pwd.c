#include <unistd.h>   // Per getcwd
#include <limits.h>   // Per PATH_MAX
#include "libft.h"   // Per ft_putendl_fd

// --- CHECCO: inizio funzione ft_pwd ---
// Stampa la directory corrente
int ft_pwd(char **args)
{
    char cwd[PATH_MAX]; // Buffer per la directory corrente
    (void)args; // Argomenti non usati
    if (getcwd(cwd, PATH_MAX)) { // Ottiene la directory corrente
        ft_putendl_fd(cwd, 1); // Stampa su stdout
        return 0; // Successo
    }
    ft_putendl_fd("pwd: error retrieving current directory", 2); // Errore
    return 1;
}
// --- CHECCO: fine funzione ft_pwd ---
