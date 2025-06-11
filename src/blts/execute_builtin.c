#include "../minishell.h"  // Include l'header principale con le definizioni necessarie

// Prototipi delle funzioni built-in (implementate da Checco)
// Ogni funzione prende un array di argomenti e restituisce un intero (status)
extern int ft_echo(char **args);    // Stampa gli argomenti
extern int ft_cd(char **args);      // Cambia directory
extern int ft_pwd(char **args);     // Stampa directory corrente
extern int ft_export(char **args);  // Esporta variabili d'ambiente
extern int ft_unset(char **args);   // Rimuove variabili d'ambiente
extern int ft_env(char **args);     // Stampa variabili d'ambiente
extern int ft_exit(char **args);    // Esce dalla shell

// Struttura che associa il nome di un comando alla sua funzione
typedef struct s_builtin {
    const char *name;    // Nome del comando (es. "echo", "cd")
    int (*func)(char **); // Puntatore alla funzione che implementa il comando
} t_builtin;

// Tabella statica che contiene tutti i comandi built-in disponibili
// L'ultimo elemento {NULL, NULL} serve come terminatore
static const t_builtin builtins[] = {
    {"echo", ft_echo},     // Comando echo
    {"cd", ft_cd},         // Comando cd
    {"pwd", ft_pwd},       // Comando pwd
    {"export", ft_export}, // Comando export
    {"unset", ft_unset},   // Comando unset
    {"env", ft_env},       // Comando env
    {"exit", ft_exit},     // Comando exit
    {NULL, NULL}           // Terminatore della tabella
};

/*
  Esegue un comando built-in.
  cmd: struttura che contiene il comando e i suoi argomenti
  La funzione cerca il comando nella tabella builtins e ne esegue la funzione corrispondente
*/
void execute_builtin(t_command *cmd)
{
    const char *cmd_name;  // Nome del comando da eseguire
    int i;                 // Contatore per il ciclo

    // Controllo di sicurezza: verifica che cmd e i suoi argomenti siano validi
    if (!cmd || !cmd->argv || !cmd->argv[0])
        return;

    cmd_name = cmd->argv[0];  // Prendi il nome del comando dal primo argomento
    
    // Cerca il comando nella tabella dei built-in
    for (i = 0; builtins[i].name != NULL; i++) {
        if (strcmp(cmd_name, builtins[i].name) == 0) {  // Se trova il comando
            // Esegui la funzione built-in corrispondente e salva il suo status
            g_state.last_status = builtins[i].func(cmd->argv);
            return;  // Esci dopo l'esecuzione
        }
    }
    
    // Se arriviamo qui, il comando non è stato trovato
    // Questo non dovrebbe mai succedere perché is_builtin è 1
    g_state.last_status = 1;  // Imposta status di errore
} 