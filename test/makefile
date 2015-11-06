###################################
# Copyright (c) 2015 Florian Evaldsson <florian.evaldsson@telia.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
###################################

###################################
#	General makefile for all tests
###################################

CC = gcc
NAME = test
DEBUG_TOOL = ddd

###################################
#	Libs and such
###################################

ifdef DEBUG
DEBUG_OPTIONS = DEBUG=1
else
DEBUG_OPTIONS =
endif

CFLAGS ?= 
CFLAGS += -std=gnu99 -Wall -g
CFLAGS += -I./../../src/ 
CFLAGS += $(shell pkg-config --cflags glib-2.0)

LDFLAGS ?=
LDFLAGS += -lm ./../../src/libeksparent.so.0.1
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
	rm -f ./*.s
	rm $(NAME)
	
valgrind: all
	valgrind  --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(NAME)
	
debug: all
	$(DEBUG_TOOL) ./$(NAME) -x ./../debug_profile
	
asm:
	$(CC) $(CFLAGS) -O0 -S -DASM_OUT ./test.c -o ./test0.s
	$(CC) $(CFLAGS) -O1 -S -DASM_OUT ./test.c -o ./test1.s
	$(CC) $(CFLAGS) -O2 -S -DASM_OUT ./test.c -o ./test2.s
	$(CC) $(CFLAGS) -O3 -S -DASM_OUT ./test.c -o ./test3.s

lib:
	make -C ./../../src/ all $(DEBUG_OPTIONS)
	
cleanlib:
	make -C ./../../src/ clean
	
