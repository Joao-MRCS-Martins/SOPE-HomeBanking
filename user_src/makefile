CC = gcc
RM=rm -f

CFLAGS=-g -Wall -Werror -Wextra -I. -D_GNU_SOURCE
LDFLAGS=-g
LDLIBS=

#Depend file stuff
DEPFILE=.depend
DEPTEXT='\#MAKE .DEPEND FILE'

#Defining sources and objects
SRCS=../auxiliary_code/log.c ../auxiliary_code/show_info.c $(wildcard *.c)
OBJS=$(subst .c,.o,$(SRCS))

all: main

main: $(OBJS)
	$(CC) $(LDFLAGS) -o user $(OBJS) $(LDLIBS)

depend: $(DEPFILE)

$(DEPFILE): $(SRCS)
	$(RM) $(DEPFILE)
	echo $(DEPTEXT) > $(DEPFILE)
	$(CC) $(CFLAGS) -MM $^>>$(DEPFILE);

clean:
	$(RM) $(OBJS) user .depend

distclean: clean
	$(RM) *~ $(DEPFILE)

include $(DEPFILE)
