#
# Name: Chris Miller
# Date: 11/8/2019
#
#

CC=gcc
CFLAGS=-g -Wall
FILES=Simulation.c queue.c

DERIV=${FILES:.c=.o}
DEPEND=$(DERIV)

all: Simulation
Simulation: $(DEPEND)
	$(CC) -o Simulation $(CFLAGS) $(DERIV)
	
clean:
	rm -f $(DERIV) Simulation
