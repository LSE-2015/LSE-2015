CC=gcc
SERIAL=serial.o 
COM = comNodoCentral.o
READ=read.o
SEND=send.o
TARGET1=read
TARGET2=send


all:$(TARGET1) $(TARGET2)

$(TARGET1):$(SERIAL) $(COM) $(READ)
	$(CC) -o $(TARGET1) $(SERIAL) $(COM) $(READ)

$(TARGET2):$(SERIAL) $(COM) $(SEND)
	$(CC) -o $(TARGET2) $(SERIAL) $(COM) $(SEND)

$(SERIAL):serial.c
	$(CC) -c -o $(SERIAL) serial.c

$(COM):comNodoCentral.c
	$(CC) -c -o $(COM) comNodoCentral.c

$(READ):read.c
	$(CC) -c -o $(READ) read.c

$(SEND):send.c
	$(CC) -c -o $(SEND) send.c

clean:
	$(RM) *.o *~ $(TARGET)

