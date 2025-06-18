#include "minishell.h"
#include <string.h>
#include <sys/stat.h>

/*
** Verifica se un file esiste ed è eseguibile
*/
static int is_executable(const char *path)
{
    struct stat st;
    
    if (stat(path, &st) == 0)
    {
        if (S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR))
            return (1);
    }
    return (0);
}

/*
** Cerca un eseguibile nei percorsi specificati in PATH
** Ritorna il percorso completo se trovato, NULL altrimenti
*/
char *find_executable(const char *cmd)
{
    char *path_env;
    char *path;
    char *token;
    char *full_path;
    
    // Se il comando contiene '/', è già un percorso
    if (strchr(cmd, '/'))
    {
        if (is_executable(cmd))
            return (strdup(cmd));
        return (NULL);
    }
    
    // Ottieni la variabile PATH
    path_env = getenv("PATH");
    if (!path_env)
        return (NULL);
    
    // Copia PATH per non modificare l'originale
    path = strdup(path_env);
    if (!path)
        return (NULL);
    
    // Cerca in ogni directory in PATH
    token = strtok(path, ":");
    while (token)
    {
        // Costruisci il percorso completo
        full_path = malloc(strlen(token) + strlen(cmd) + 2);
        if (!full_path)
        {
            free(path);
            return (NULL);
        }
        
        sprintf(full_path, "%s/%s", token, cmd);
        
        // Verifica se il file esiste ed è eseguibile
        if (is_executable(full_path))
        {
            free(path);
            return (full_path);
        }
        
        free(full_path);
        token = strtok(NULL, ":");
    }
    
    free(path);
    return (NULL);
} 