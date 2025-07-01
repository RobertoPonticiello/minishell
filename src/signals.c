#include "minishell.h"
#include <signal.h>
#include <string.h>
#include <stddef.h>

t_shell_state g_state = {0, 0};

/*
  Signal handler per SIGINT. Imposta g_state.signal e ristampa il prompt.
  NOTA: write è async-signal-safe, quindi va bene chiamarlo qui.
*/
static void handle_sigint(int signo)
{
    (void)signo;
    g_state.signal = SIGINT;
    /* Stampa una newline per andare a capo */
    write(STDOUT_FILENO, "\n", 1);
    /* Pulisci il buffer di input */
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

/*
  setup_signals: configura l'handler per SIGINT e ignora SIGQUIT.
*/
void setup_signals(void)
{
    struct sigaction sa;

    /* 1) Installiamo handle_sigint per SIGINT, con SA_RESTART */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; 
    sigaction(SIGINT, &sa, NULL);

    /* 2) Ignoriamo SIGQUIT (Ctrl-\) */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGQUIT, &sa, NULL);
}
