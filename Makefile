NAME = ft_ping

SRC = $(shell find . -name '*.c')
INC = $(shell find . -name '*.h')

OBJ_DIR = objects
OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))

CC = gcc

GREEN = \033[0;32m
RED = \033[0;31m
NC = \033[0m
RM = rm -f

all: $(NAME)

$(OBJ_DIR)/%.o: %.c $(INC)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(OBJ:.o=.d)

$(NAME): $(OBJ)
	@echo "$(GREEN)Compiling...$(NC)"
	@sudo $(CC) $(CFLAGS) -o $(NAME) $(OBJ)
	@echo "$(GREEN)Compiled.$(NC)"

clean:
	@$(RM) $(OBJ) $(OBJ:.o=.d)
	@$(RM) -r $(OBJ_DIR)
	@echo "$(RED)Cleaned up object files!$(NC)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)Removed executable!$(NC)"

re: fclean all
