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

# === Compiler & flags ===
CC       := gcc
CFLAGS   := -Wall -Wextra -Werror -g -I$(INCDIR) -I$(LIBFTDIR) -I$(PRTFDIR)
LDFLAGS  := -L$(LIBFTDIR) -lft -L$(PRTFDIR) -lftprintf -lreadline

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
