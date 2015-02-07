###################################
#	Makefile for the test
###################################

CC = gcc
NAME = test

###################################
#	Libs and such
###################################

CFLAGS = -std=c99 -Wall -g
CFLAGS += -I./../src/ 
CFLAGS += $(shell pkg-config --cflags glib-2.0)

LDFLAGS = -lm ./../src/libeks.so.0.1
LDFLAGS += $(shell pkg-config --libs glib-2.0)

###################################
#	DECLARING OBJECTS AND SUCH
###################################

SRCS=$(wildcard *.c)

OBJS=$(SRCS:.c=.o)

###################################
#	Common compile options
###################################

all: $(NAME)

$(NAME): $(OBJS) 
	$(CC) -o $(NAME) $(LDFLAGS) $^
	
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
	
run: all
	./$(NAME)

clean:
	rm -f ./*.o
	rm $(NAME)
	
valgrind: all
	valgrind  --track-origins=yes --tool=memcheck --leak-check=full ./$(NAME)
