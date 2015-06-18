CC=gcc
SERIAL=serial.o 
COM = comNodoCentral.o
EXAMPLE=example.o
TARGET=example


all:$(TARGET)

$(TARGET):$(SERIAL) $(COM) $(EXAMPLE)
	$(CC) -o $(TARGET) $(SERIAL) $(COM) $(EXAMPLE)

$(SERIAL):serial.c
	$(CC) -c -o $(SERIAL) serial.c

$(COM):comNodoCentral.c
	$(CC) -c -o $(COM) comNodoCentral.c

$(EXAMPLE):example.c
	$(CC) -c -o $(EXAMPLE) example.c

clean:
	$(RM) *.o *~ $(TARGET)

