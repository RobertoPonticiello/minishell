# === Directory principali ===
SRCDIR   := src
INCDIR   := include
OBJDIR   := obj
LIBFTDIR := libft
PRTFDIR  := printf

# === Output ===
NAME     := minishell
LIBFT    := $(LIBFTDIR)/libft.a
PRINTF   := $(PRTFDIR)/libftprintf.a

# === Rilevamento sistema operativo ===
UNAME_S := $(shell uname -s)

# === Compiler & flags ===
CC       := gcc
CFLAGS   := -Wall -Wextra -Werror -g -I$(INCDIR) -I$(LIBFTDIR) -I$(PRTFDIR)
LDFLAGS  := -L$(LIBFTDIR) -lft -L$(PRTFDIR) -lftprintf

# === Configurazione specifica per sistema operativo ===
ifeq ($(UNAME_S),Darwin)
	# macOS con Homebrew
	READLINE_PATH := $(shell brew --prefix readline 2>/dev/null)
	ifneq ($(READLINE_PATH),)
		CFLAGS += -I$(READLINE_PATH)/include
		LDFLAGS += -L$(READLINE_PATH)/lib
	else
		# Fallback se brew non funziona
		CFLAGS += -I/opt/homebrew/Cellar/readline/8.2.13/include
		LDFLAGS += -L/opt/homebrew/Cellar/readline/8.2.13/lib
	endif
	LDFLAGS += -lreadline
else ifeq ($(UNAME_S),Linux)
	# Linux - prova diversi percorsi comuni
	LDFLAGS += -lreadline
	# Aggiungi percorsi comuni se readline non è nel path standard
	ifneq ($(wildcard /usr/include/readline),)
		# readline è installato nel path standard
	else ifneq ($(wildcard /usr/local/include/readline),)
		CFLAGS += -I/usr/local/include
		LDFLAGS += -L/usr/local/lib
	endif
endif

# === Ricerca ricorsiva dei file .c ===
SRCS := $(shell find $(SRCDIR) -type f -name '*.c')
OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

# === Target default ===
all: $(NAME)

# === Linking finale ===
$(NAME): $(LIBFT) $(PRINTF) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

# === Compilazione sorgenti in obj/... ===
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# === Costruzione librerie ===
$(LIBFT):
	@$(MAKE) -C $(LIBFTDIR)

$(PRINTF):
	@$(MAKE) -C $(PRTFDIR)

# === Utility ===
clean:
	@rm -rf $(OBJDIR)
	@$(MAKE) -C $(LIBFTDIR) clean
	@$(MAKE) -C $(PRTFDIR) clean

fclean: clean
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFTDIR) fclean
	@$(MAKE) -C $(PRTFDIR) fclean

re: fclean all

.PHONY: all clean fclean re
