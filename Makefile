NAME        := minishell
CC          := gcc
CFLAGS      := -ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable 
LFLAGS      := -lncurses -lreadline -ggdb -fsanitize=address
HEADER_DIRS := $(shell fd -t file --regex '.*\.h' src lib | grep -o '.*/' | grep -v '/.*\.h' | sort  | uniq)
INCLUDES    := $(addprefix -I, $(HEADER_DIRS))
SRCS        := $(shell fd -t file --regex '.*\.c' src | grep -v 'main')
OBJS        := $(SRCS:.c=.o)
LIBS        := lib/getline/getline.a

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(NAME): $(OBJS) $(LIBS)
	$(CC) $(INCLUDES) -o $@ src/main.c $^ $(LFLAGS) 
$(LIBS): #do I use %.a: instead?
	make -C $(dir $@) re
all: $(NAME)
clean:
	rm -f $(OBJS) $(TEST_BINS)
fclean: clean
	rm -f $(NAME) $(LIBS)

#TODO: get rid of sleep (for rm to finish and makefile that falsely will index a file)
#funnily, single core wont have the bug.
re: fclean
	sleep 0.42; make all

# testing:
TEST_SRCS := $(shell fd -t file --regex '.*main.*\.c')
TEST_BINS := $(TEST_SRCS:.c=.out)
$(TEST_BINS): $(OBJS) 
%.out: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LFLAGS)
tags:
	ctags -R src/*

.PHONY: all clean fclean re tags $(NAME) $(LIBS)
