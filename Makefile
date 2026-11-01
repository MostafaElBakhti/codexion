NAME := codexion

CC := cc
CFLAGS := -Wall -Wextra -Werror -pthread

SRCS := src/main.c \
	src/parsing.c \
	src/init.c \
	src/dongle.c \
	src/reservation.c \
	src/heap.c \
	src/heap_order.c \
	src/thread.c \
	src/state.c \
	src/monitor.c \
	src/acquire.c \
	src/start.c \
	src/time.c

OBJS := $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c src/codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean
	$(MAKE) all

.PHONY: all clean fclean re
