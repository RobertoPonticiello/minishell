#include <unistd.h>   // Per chdir, getcwd
#include <stdio.h>    // Per perror, fprintf
#include <string.h>   // Per strcmp
#include <stdlib.h>   // Per getenv

// --- CHECCO: inizio funzione ft_cd ---
// Cambia directory, gestisce cd, cd -, errori e stampa su stderr
int ft_cd(char **args)
{
    char *path = NULL;
    
    // Controlla se ci sono troppi argomenti
    if (args[1] && args[2]) {
        fprintf(stderr, "cd: troppi argomenti\n");
        return 1;
    }
    
    if (!args[1] || strcmp(args[1], "~") == 0) // cd o cd ~
        path = getenv("HOME"); // Usa $HOME
    else if (strcmp(args[1], "-") == 0) // cd -
        path = getenv("OLDPWD"); // Usa $OLDPWD
    else
        path = args[1]; // cd <path>
    if (!path) {
        fprintf(stderr, "cd: path not set\n"); // Errore se path non trovato
        return 1;
    }
    if (chdir(path) != 0) { // Cambia directory
        perror("cd"); // Stampa errore
        return 1;
    }
    return 0; // Successo
}
// --- CHECCO: fine funzione ft_cd ---
