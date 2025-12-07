NAME	 := getline.a
CFLAGS	 := -Wall -Wextra -Werror -g
CC       := gcc
SRC		 := getline.c getline_extras.c 
OBJ		 := $(SRC:.c=.o)
SRC_TEST    := $(shell find tests/ -type f -regex ".*\.c")
EXE_TEST := $(SRC_TEST:.c=.out)

$(NAME): $(OBJ)
	ar rcs $@ $^
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
all: $(NAME) $(EXE_TEST)
clean:
	rm -f $(OBJ)
fclean: clean
	rm -f $(NAME)
	rm -f $(EXE_TEST)
re: fclean 
	make all
#testing:
%.out: %.c $(NAME)
	$(CC) $(CFLAGS) -o $@ $^ $(NAME)
test: $(EXE_TEST)

.PHONY: test all clean fclean re $(NAME)
