#include <stdio.h>    // Per fprintf
#include <stdlib.h>   // Per exit, atoi
#include <string.h>   // Per strlen
#include <ctype.h>    // Per isdigit

// Controlla se una stringa rappresenta un numero valido (con possibile segno)
static int is_valid_number(const char *str)
{
    int i = 0;
    
    if (!str || !str[0])
        return 0;
    
    // Salta eventuale segno + o -
    if (str[i] == '+' || str[i] == '-')
        i++;
    
    // Deve esserci almeno una cifra dopo il segno
    if (!str[i])
        return 0;
    
    // Tutti i caratteri rimanenti devono essere cifre
    while (str[i])
    {
        if (!isdigit(str[i]))
            return 0;
        i++;
    }
    return 1;
}

// --- CHECCO: inizio funzione ft_exit ---
// Gestisce il comando exit con vari casi
int ft_exit(char **args)
{
    int code = 0;
    
    if (args[1] && args[2]) // Troppi argomenti
    {
        fprintf(stderr, "exit: too many arguments\n");
        return 1;
    }
    
    if (args[1]) // Se c'è un argomento
    {
        if (!is_valid_number(args[1]))
        {
            fprintf(stderr, "exit: %s: è necessario un argomento numerico\n", args[1]);
            exit(2);
        }
        code = atoi(args[1]);
        // In bash, exit code è sempre modulo 256 (unsigned byte)
        code = code & 0xFF;
    }
    
    exit(code);
}
// --- CHECCO: fine funzione ft_exit ---
