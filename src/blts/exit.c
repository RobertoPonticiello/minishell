#include <stdio.h>    // Per fprintf
#include <stdlib.h>   // Per exit, atoi
#include <string.h>   // Per isdigit

// --- CHECCO: inizio funzione ft_exit ---
// Gestisce il comando exit con vari casi
int ft_exit(char **args)
{
    int code = 0; // Codice di uscita di default
    if (args[1] && args[2]) { // Troppi argomenti
        fprintf(stderr, "exit: too many arguments\n");
        return 1;
    }
    if (args[1]) { // Se c'è un argomento
        for (int i = 0; args[1][i]; i++) // Controlla che sia numerico
            if (args[1][i] < '0' || args[1][i] > '9') {
                fprintf(stderr, "exit: %s: numeric argument required\n", args[1]);
                exit(255);
            }
        code = atoi(args[1]); // Converte l'argomento in intero
    }
    exit(code); // Termina il programma
}
// --- CHECCO: fine funzione ft_exit ---
