
# Basic parameters
CC     = gcc 
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O3
OBJS   = trike7.o

###############################################################################
#                                                                             #
#  make : to compile the code, update docs, run the tests, clean the folder.  #
#                                                                             #
###############################################################################

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

trike7: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
	./trike7 > log.txt
	/bin/rm -rf *.o *~
	/bin/rm -rf trike7
