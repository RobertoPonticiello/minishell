/*
   -----------------------------
   Suddivisione dei compiti:

   - Bobby (esecuzione & segnali):
     1. setup_signals()
     2. execute_pipeline(t_command *commands)
     3. cleanup_after_execution(t_command *commands)

   - Checco (parsing):
     1. parse_input(const char *line)
     2. free_commands(t_command *commands)
   -----------------------------
*/

#include "minishell.h" 

int main(void)
{
	char *line;
	t_command *commands;

	/* 1) Setup segnali (tua parte) */
	setup_signals();

	/* 2) Loop principale della shell */
	while (1) {
		/* a) Leggi input utente */
		line = readline("minishell$ ");
		if (!line) {
			printf("exit\n");
			break;
		}
		if (*line)
			add_history(line);

		/* b) Reset segnale */
		g_state.signal = 0;

		/* c) Parsing input (Persona 1) */
		commands = parse_input(line);
		free(line);
		if (!commands)
			continue;

		/* d) Esecuzione pipeline (tua parte) */
		execute_pipeline(commands);

		/* e) Cleanup memorie e risorse (tua parte) */
		cleanup_after_execution(commands);
		}

	return g_state.last_status;
}
