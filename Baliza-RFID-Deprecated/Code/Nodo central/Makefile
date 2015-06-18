CC=gcc
SERIAL=serial.o 
COM = comNodoCentral.o
NODO=nodoCentral.o
TARGET=nodoCentral


all:$(TARGET)

$(TARGET):$(SERIAL) $(COM) $(NODO)
	$(CC) -o $(TARGET) $(SERIAL) $(COM) $(NODO)

$(SERIAL):serial.c
	$(CC) -c -o $(SERIAL) serial.c

$(COM):comNodoCentral.c
	$(CC) -c -o $(COM) comNodoCentral.c

$(NODO):nodoCentral.c
	$(CC) -c -o $(NODO) nodoCentral.c

clean:
	$(RM) *.o *~ $(TARGET)

