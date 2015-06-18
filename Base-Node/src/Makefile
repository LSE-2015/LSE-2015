CC=gcc
CFLAGS=-g -Wall -Werror -O2
LDLIBS=-lwiringPi 
SOFT=nCentral.o
TARGET=nCentral


all:$(TARGET)

$(TARGET):$(SOFT)
	$(CC) $(CFLAGS) -o $(TARGET) fsm.o $(SOFT) $(LDLIBS)

$(SOFT):nCentral.c
	$(CC) $(CFLAGS) -c -o fsm.o fsm.c $(LDLIBS)
	$(CC) $(CFLAGS) -c -o $(SOFT) nCentral.c $(LDLIBS)
clean:
	$(RM) *.o *~ $(TARGET)

