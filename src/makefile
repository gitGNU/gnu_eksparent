###################################
#	SEE LICENSE FOR LICENSE
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
