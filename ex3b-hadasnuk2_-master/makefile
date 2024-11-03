CC = gcc
CFLAGS = -Wall -Wextra

# For make tweets
TWEETS_SRC = markov_chain.c markov_chain.h tweets_generator.c
TWEETS_OBJ = markov_chain.o tweets_generator.o

# For make_snakes
SNAKE_SRC = snakes_and_ladders.c markov_chain.c markov_chain.h
SNAKE_OBJ = snakes_and_ladders.o markov_chain.o

# For linked_list
LINKED_LIST_SRC = linked_list.c linked_list.h
LINKED_LIST_OBJ = linked_list.o

all: tweets make_snake

tweets: $(TWEETS_OBJ) $(LINKED_LIST_OBJ)
	$(CC) $(CFLAGS) $(TWEETS_OBJ) $(LINKED_LIST_OBJ) -o tweets_generator

make_snake: $(SNAKE_OBJ) $(LINKED_LIST_OBJ)
	$(CC) $(CFLAGS) $(SNAKE_OBJ) $(LINKED_LIST_OBJ) -o snakes_and_ladders

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f tweets make_snake *.o