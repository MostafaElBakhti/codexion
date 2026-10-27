NAME := codexion

CC := cc
CFLAGS := -Wall -Wextra -Werror -pthread
DEPFLAGS := -MMD -MP

SRC_DIR := src
OBJ_DIR := obj

SRC_FILES := main.c \
	parsing.c \
	init.c \
	dongle.c \
	reservation.c \
	heap.c \
	heap_order.c \
	thread.c \
	acquire.c \
	start.c \
	time.c

SRCS := $(addprefix $(SRC_DIR)/,$(SRC_FILES))
OBJS := $(SRC_FILES:%.c=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
