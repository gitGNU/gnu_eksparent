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
#	COMMON CONFIGURATION
###################################

NAME = eksparent
VERSION = 0.1

###################################
#	GENERAL STUFF
###################################

CC = gcc

###################################
#	C-FLAGS
#	LD FLAGS
###################################

CFLAGS = -c -fPIC -std=c99 
CFLAGS += -g -Wall
CFLAGS += -DCC_COMPILING_EKS
CFLAGS += $(shell pkg-config --cflags glib-2.0)

LDFLAGS = -lm
LDFLAGS += $(shell pkg-config --libs glib-2.0)

###################################
#	DECLARING OBJECTS AND SUCH
###################################

SRCS=$(wildcard *.c)

OBJS=$(SRCS:.c=.o)

LIBNAME=lib$(NAME).so.$(VERSION)

###################################
#	ALL BUILD OPTIONS
###################################

all: $(NAME)

$(NAME): $(OBJS) 
	$(CC) -shared -o $(LIBNAME) $(LDFLAGS) $^
	
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
	
clean:
	rm -f ./*.o
	rm -f ./*.so*

install: all
	cp $(NAME).h /usr/include/$(NAME).h
	cp $(LIBNAME) /usr/lib/$(LIBNAME)
	
ifeq ("$(wildcard /usr/lib/$(LIBNAME:.so.$(VERSION)=.so))","")
	ln -s /usr/lib/$(LIBNAME) /usr/lib/$(LIBNAME:.so.$(VERSION)=.so)
endif
	
uninstall: clean
	rm /usr/include/$(NAME).h
	rm /usr/lib/$(LIBNAME)
	rm /usr/lib/$(LIBNAME:.so.$(VERSION)=.so)
	
doc:
	doxygen ./../doc/doxygen_conf
