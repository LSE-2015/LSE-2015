CC=gcc
SERIAL=serial.o 
COM = comNodoCentral.o
EXAMPLE=print_test.o
TARGET=print_test


all:$(TARGET)

$(TARGET):$(SERIAL) $(COM) $(EXAMPLE)
	$(CC) -o $(TARGET) $(SERIAL) $(COM) $(EXAMPLE)

$(SERIAL):serial.c
	$(CC) -c -o $(SERIAL) serial.c

$(COM):comNodoCentral.c
	$(CC) -c -o $(COM) comNodoCentral.c

$(EXAMPLE):print_test.c
	$(CC) -c -o $(EXAMPLE) print_test.c

clean:
	$(RM) *.o *~ $(TARGET)

