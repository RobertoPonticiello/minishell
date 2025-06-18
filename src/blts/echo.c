#include <unistd.h> // Per write
#include <string.h> // Per strcmp

// --- CHECCO: inizio funzione ft_echo ---
// Stampa gli argomenti separati da spazio, gestisce l'opzione -n
int ft_echo(char **args)
{
    int i = 1; // Indice per scorrere gli argomenti
    int n_option = 0; // Flag per l'opzione -n
    // Controlla se c'è l'opzione -n
    while (args[i] && strcmp(args[i], "-n") == 0) {
        n_option = 1;
        i++;
    }
    // Stampa tutti gli argomenti separati da spazio
    while (args[i]) {
        write(1, args[i], strlen(args[i])); // Stampa l'argomento
        if (args[i + 1]) // Se non è l'ultimo, stampa uno spazio
            write(1, " ", 1);
        i++;
    }
    // Se non c'è -n, stampa il newline
    if (!n_option)
        write(1, "\n", 1);
    return 0; // Successo
}
// --- CHECCO: fine funzione ft_echo ---
