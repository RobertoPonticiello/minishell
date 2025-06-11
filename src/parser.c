#include <fcntl.h> // (CHECCO) Per i flag di open
#include "minishell.h"

// Tokenizza la linea di comando in una lista di token
static t_token *tokenize(const char *line);

// Gestisce le virgolette e crea i token corretti
static void handle_quotes(t_token **tokens);

// Espande le variabili d'ambiente e $?
static void expand_variables(t_token **tokens);

// Analizza pipe e redirezioni e costruisce la pipeline
static t_command *build_commands(t_token *tokens);

// Gestisce errori di sintassi
static int check_syntax_errors(t_token *tokens);

// Libera la lista di token
static void free_tokens(t_token *tokens);

// --- CHECCO: start variable expansion helpers ---
// Espande $VAR e $? in una stringa
static int copy_env_value(const char *src, int *si, char *dst, int *di)
{
    // (CHECCO) Gestisce $VAR e $?
    if (src[*si + 1] == '?') { // Se trova $?
        char status[12];
        snprintf(status, sizeof(status), "%d", g_state.last_status); // Converte lo status in stringa
        for (int k = 0; status[k]; k++) dst[(*di)++] = status[k]; // Copia lo status nel buffer
        *si += 2; // Avanza di 2 caratteri ($?)
        return 1; // Espansione fatta
    }
    int var_start = *si + 1, var_len = 0; // Inizio e lunghezza del nome variabile
    while (src[var_start + var_len] && (isalnum(src[var_start + var_len]) || src[var_start + var_len] == '_'))
        var_len++; // Conta la lunghezza del nome variabile
    if (var_len > 0) {
        char var[256];
        strncpy(var, src + var_start, var_len); var[var_len] = 0; // Copia il nome variabile
        char *val = getenv(var); // Prende il valore della variabile d'ambiente
        if (val) for (int k = 0; val[k]; k++) dst[(*di)++] = val[k]; // Copia il valore nel buffer
        *si = var_start + var_len; // Avanza l'indice sorgente
        return 1; // Espansione fatta
    }
    return 0; // Nessuna espansione
}

// Espande le variabili in una stringa e restituisce una nuova stringa
static char *expand_string(const char *src)
{
    char *dst = malloc(4096); // (CHECCO) Buffer di destinazione
    int si = 0, di = 0; // Indici per sorgente e destinazione
    while (src[si]) { // Scorri la stringa sorgente
        if (src[si] == '$' && src[si + 1]) { // Se trova una variabile
            if (copy_env_value(src, &si, dst, &di)) continue; // Espandi e continua
        }
        dst[di++] = src[si++]; // Copia carattere normale
    }
    dst[di] = 0; // Termina la stringa
    return dst; // Ritorna la stringa espansa
}
// --- CHECCO: end variable expansion helpers ---

// --- CHECCO: start expand_variables ---
// Scorre i token e applica l'espansione delle variabili
static void expand_variables(t_token **tokens)
{
    t_token *curr = *tokens; // (CHECCO) Puntatore al primo token
    while (curr) { // Scorri tutti i token
        if (curr->type == TOKEN_WORD && curr->value) { // Solo per token parola
            char *expanded = expand_string(curr->value); // Espandi variabili
            free(curr->value); // Libera la vecchia stringa
            curr->value = strdup(expanded); // Aggiorna il valore del token
            free(expanded); // Libera il buffer temporaneo
        }
        curr = curr->next; // Passa al prossimo token
    }
}
// --- CHECCO: end expand_variables ---

// --- CHECCO: inizio funzioni tokenizzazione ---

// (CHECCO) Crea un nuovo token e lo aggiunge in coda alla lista
static t_token *create_token(const char *val, t_token_type type)
{
    t_token *tok = malloc(sizeof(t_token)); // Alloca memoria per un nuovo token
    if (!tok) return NULL;                  // Se malloc fallisce, ritorna NULL
    tok->value = strdup(val);               // Copia il valore del token
    tok->type = type;                       // Imposta il tipo di token
    tok->next = NULL;                       // Inizializza il puntatore al prossimo token a NULL
    return tok;                             // Ritorna il nuovo token
}

// (CHECCO) Riconosce e gestisce operatori speciali (|, <, >, <<, >>)
static int is_operator(const char *line, int i, t_token **tok)
{
    if (line[i] == '|' ) {                          // Se trova una pipe
        *tok = create_token("|", TOKEN_PIPE); // Crea token pipe
        return 1;                                  // Consuma 1 carattere
    }
    if (line[i] == '<' && line[i+1] == '<') {      // Se trova <<
        *tok = create_token("<<", TOKEN_HEREDOC); // Crea token heredoc
        return 2;                                  // Consuma 2 caratteri
    }
    if (line[i] == '>' && line[i+1] == '>') {      // Se trova >>
        *tok = create_token(">>", TOKEN_APPEND); // Crea token append
        return 2;                                  // Consuma 2 caratteri
    }
    if (line[i] == '<') {                          // Se trova <
        *tok = create_token("<", TOKEN_REDIR_IN); // Crea token redir input
        return 1;                                  // Consuma 1 carattere
    }
    if (line[i] == '>') {                          // Se trova >
        *tok = create_token(">", TOKEN_REDIR_OUT); // Crea token redir output
        return 1;                                  // Consuma 1 carattere
    }
    return 0;                                      // Nessun operatore trovato
}

// (CHECCO) Estrae una parola o una stringa tra virgolette
static int extract_word(const char *line, int i, char **out)
{
    int start = i;                                 // Salva la posizione iniziale
    int len = 0;                                   // Lunghezza della parola
    if (line[i] == '\'' || line[i] == '"') {     // Se inizia con una virgoletta
        char quote = line[i++];                    // Salva il tipo di virgoletta e avanza
        start = i;                                // Nuovo inizio dopo la virgoletta
        while (line[i] && line[i] != quote) i++;   // Avanza fino alla chiusura
        len = i - start;                          // Calcola la lunghezza
        *out = strndup(line + start, len);        // Copia la parola tra virgolette
        if (line[i] == quote) i++;                // Salta la virgoletta di chiusura
        return i - start + 1;                     // Ritorna caratteri consumati
    }
    while (line[i] && line[i] != ' ' && line[i] != '\t' && // Finché non trova separatori
           line[i] != '|' && line[i] != '<' && line[i] != '>' &&
           line[i] != '\'' && line[i] != '"') {
        i++;                                      // Avanza
    }
    len = i - start;                              // Calcola la lunghezza
    *out = strndup(line + start, len);            // Copia la parola
    return len;                                   // Ritorna caratteri consumati
}

// (CHECCO) Funzione principale di tokenizzazione
static t_token *tokenize(const char *line)
{
    int i = 0;                                    // Indice sulla stringa
    t_token *head = NULL, *tail = NULL;           // Testa e coda della lista token
    while (line[i]) {                             // Finché non finisce la stringa
        while (line[i] == ' ' || line[i] == '\t') i++; // Salta spazi e tab
        if (!line[i]) break;                      // Se fine stringa, esci
        t_token *tok = NULL;                      // Token temporaneo
        int op_len = is_operator(line, i, &tok); // Prova a riconoscere un operatore
        if (op_len) {                             // Se trovato operatore
            if (!head) head = tok; else tail->next = tok; // Aggiungi in lista
            tail = tok;                           // Aggiorna coda
            i += op_len;                          // Avanza indice
            continue;                             // Passa al prossimo ciclo
        }
        char *word = NULL;                        // Buffer per parola
        int wlen = extract_word(line, i, &word); // Estrai parola/virgolettato
        if (wlen > 0 && word && *word) {          // Se parola valida
            tok = create_token(word, TOKEN_WORD); // Crea token parola
            if (!head) head = tok; else tail->next = tok; // Aggiungi in lista
            tail = tok;                           // Aggiorna coda
        }
        free(word);                               // Libera buffer temporaneo
        i += wlen;                                // Avanza indice
    }
    return head;                                  // Ritorna la lista di token
}
// --- CHECCO: fine funzioni tokenizzazione ---

// --- CHECCO: inizio funzione handle_quotes ---
// (CHECCO) Gestisce le virgolette nei token (placeholder, da espandere se serve)
static void handle_quotes(t_token **tokens)
{
    t_token *curr = *tokens; // Puntatore al primo token
    while (curr) {
        // Qui si potrebbe gestire la rimozione delle virgolette dai token
        // e l'unione di token se necessario (ad esempio "ciao mondo")
        // Per ora, lasciamo i token così come sono
        // (La logica può essere espansa in seguito se richiesto dal parsing avanzato)
        curr = curr->next; // Passa al prossimo token
    }
    // Nota: la funzione è un placeholder per rispettare la struttura modulare
}
// --- CHECCO: fine funzione handle_quotes ---

// --- CHECCO: inizio funzione check_syntax_errors ---
// Controlla errori di sintassi nei token
static int check_syntax_errors(t_token *tokens)
{
    t_token *curr = tokens; // Puntatore al primo token
    if (!curr) return 1; // Errore: linea vuota
    if (curr->type == TOKEN_PIPE) return 1; // Errore: pipe all'inizio
    while (curr) {
        if (curr->type == TOKEN_PIPE && (!curr->next || curr->next->type == TOKEN_PIPE))
            return 1; // Errore: pipe doppia o finale
        if ((curr->type == TOKEN_REDIR_IN || curr->type == TOKEN_REDIR_OUT ||
             curr->type == TOKEN_HEREDOC || curr->type == TOKEN_APPEND) &&
            (!curr->next || curr->next->type != TOKEN_WORD))
            return 1; // Errore: redirezione senza argomento
        curr = curr->next; // Passa al prossimo token
    }
    return 0; // Nessun errore trovato
}
// --- CHECCO: fine funzione check_syntax_errors ---

// --- CHECCO: inizio funzione handle_redirection ---
// Gestisce le redirezioni e aggiorna i file descriptor del comando
static void handle_redirection(t_command *cmd, t_token *curr)
{
    if ((curr->type == TOKEN_REDIR_IN || curr->type == TOKEN_HEREDOC) && curr->next) {
        if (cmd->in_fd > 0) close(cmd->in_fd); // Chiudi fd precedente
        cmd->in_fd = open(curr->next->value, O_RDONLY); // Apre file input
    }
    if ((curr->type == TOKEN_REDIR_OUT || curr->type == TOKEN_APPEND) && curr->next) {
        if (cmd->out_fd > 0) close(cmd->out_fd); // Chiudi fd precedente
        int flags = O_WRONLY | O_CREAT | (curr->type == TOKEN_APPEND ? O_APPEND : O_TRUNC);
        cmd->out_fd = open(curr->next->value, flags, 0644); // Apre file output
    }
}
// --- CHECCO: fine funzione handle_redirection ---

// --- CHECCO: inizio funzione build_commands
// Costruisce la lista di comandi (pipeline) a partire dai token, gestendo le redirezioni
static t_command *build_commands(t_token *tokens)
{
    t_command *head = NULL, *tail = NULL;
    t_token *curr = tokens;
    while (curr) {
        t_command *cmd = calloc(1, sizeof(t_command));
        cmd->in_fd = -1; cmd->out_fd = -1;
        int argc = 0; char *argv[256];
        while (curr && curr->type != TOKEN_PIPE) {
            if (curr->type == TOKEN_WORD)
                argv[argc++] = strdup(curr->value);
            handle_redirection(cmd, curr); // Gestisce le redirezioni
            curr = curr->next;
        }
        argv[argc] = NULL;
        cmd->argv = malloc((argc + 1) * sizeof(char*));
        for (int i = 0; i <= argc; i++) cmd->argv[i] = argv[i];
        if (!head) head = cmd; else tail->next = cmd;
        tail = cmd;
        if (curr) curr = curr->next;
    }
    return head;
}
// --- CHECCO: fine funzione build_commands

// --- CHECCO: inizio funzione free_tokens ---
// Libera tutta la memoria allocata dalla lista di token
static void free_tokens(t_token *tokens)
{
    t_token *curr = tokens; // Puntatore al token corrente
    while (curr) { // Scorri tutta la lista
        t_token *next = curr->next; // Salva il prossimo token
        if (curr->value) free(curr->value); // Libera la stringa del token
        free(curr); // Libera la struttura del token
        curr = next; // Passa al prossimo token
    }
}
// --- CHECCO: fine funzione free_tokens ---

// --- CHECCO: inizio funzione free_commands ---
// Libera tutta la memoria allocata dalla lista di comandi t_command
void free_commands(t_command *cmds)
{
    t_command *curr = cmds; // Puntatore al comando corrente
    while (curr) { // Scorri tutta la lista
        t_command *next = curr->next; // Salva il prossimo comando
        if (curr->argv) { // Se ci sono argomenti
            for (int i = 0; curr->argv[i]; i++) // Scorri tutti gli argomenti
                free(curr->argv[i]); // Libera ogni argomento
            free(curr->argv); // Libera l'array di puntatori
        }
        if (curr->path) free(curr->path); // Libera il path se presente
        free(curr); // Libera la struttura del comando
        curr = next; // Passa al prossimo comando
    }
}
// --- CHECCO: fine funzione free_commands ---

// Funzione principale chiamata dal main
// Ritorna la lista di comandi pronta per l'esecuzione
// oppure NULL in caso di errore di sintassi

t_command *parse_input(const char *line)
{
    t_token *tokens = tokenize(line);
    if (!tokens)
        return NULL;
    handle_quotes(&tokens);
    expand_variables(&tokens);
    if (check_syntax_errors(tokens)) {
        free_tokens(tokens);
        return NULL;
    }
    t_command *cmds = build_commands(tokens);
    free_tokens(tokens);
    return cmds;
} 