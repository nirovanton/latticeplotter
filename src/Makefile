LIBS = -lm -lgraph -lX11

INCDIR = $(HOME)/include 
LIBDIR = $(HOME)/lib

#CFLAGS = -std=gnu99 --pedantic -Wall -O0 -g -I $(INCDIR)
#LDFLAGS = -std=gnu99 --pedantic -Wall -O0 -g -L $(LIBDIR)
CFLAGS = -std=gnu99 --pedantic -Wall -O3 -I $(INCDIR)
LDFLAGS = -std=gnu99 --pedantic -Wall -O3 -L $(LIBDIR)

% : %.c Makefile
	$(CC) $< $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@

*.c : 

